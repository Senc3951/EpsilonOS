#include <mem/address_space.hpp>
#include <mem/address_range.hpp>
#include <mem/pmm.hpp>
#include <arch/register.hpp>
#include <log.hpp>

namespace kernel::memory
{
AddressSpace::AddressSpace()
{
    // Allocate memory for the table
    void *pml4_frame = PhysicalMemoryManager::instance().allocate_frame();
    if (!pml4_frame)
        panic("failed allocating memory for pml4");
    
    // Reset the table
    m_pml4 = reinterpret_cast<PageTable *>(tohh(pml4_frame));
    m_pml4->reset();
    
    // Map lower 4GiB
    AddressRange _4gib_range(0, 4 * GiB);
    for (auto phys_addr : _4gib_range)
        map(tohh(phys_addr), phys_addr, PagingFlags::Writable);
    
    // Map the kernel
    AddressRange kernel_range(0, kernel_file_request.response->kernel_file->size);
    u64 kernel_phys_start = kernel_address_request.response->physical_base;
    u64 kernel_virt_start = kernel_address_request.response->virtual_base;
    for (auto offset : kernel_range)
        map(kernel_virt_start + offset, kernel_phys_start + offset, PagingFlags::Writable);
    
    // Get framebuffer physical entry
    limine_memmap_entry *framebuffer_memmap_entry = PhysicalMemoryManager::find_physical_entry([](limine_memmap_entry *entry) {
        return entry->type == LIMINE_MEMMAP_FRAMEBUFFER;
    });
    u64 framebuffer_virt_start = reinterpret_cast<u64>(framebuffer_request.response->framebuffers[0]->address);
    u64 framebuffer_phys_start = framebuffer_memmap_entry->base;
    
    // Map framebuffer
    AddressRange framebuffer_range(0, framebuffer_memmap_entry->length);
    for (auto offset : framebuffer_range)
        map(framebuffer_virt_start + offset, framebuffer_phys_start + offset, PagingFlags::Writable | PagingFlags::ExecuteDisable | WC_CACHE);
}

void AddressSpace::load()
{
    // CR3 expects the physical address of pml4
    assert(m_pml4 && "null pml4");
    Register::write(CR3, reinterpret_cast<u64>(fromhh(m_pml4)));
}

void AddressSpace::flush_tlb(const u64 virt)
{
    asm volatile("invlpg (%0)" ::"r" (virt) : "memory");
}

void AddressSpace::map(const u64 virt, const u64 phys, const u64 unfixed_flags)
{
    bool is_pat;
    u64 flags = fix_flags(unfixed_flags, is_pat);
    
    // Get the page table entry for the specified virtual address
    PageTableEntry *entry = virt2pte(virt, flags, true);

    // If remapping to a different frame, release the current one
    u64 frame = pte2frame(entry);
    if (frame && frame != phys)
        PhysicalMemoryManager::instance().release_frame(frame);
    
    if (is_pat) // Re-enable pat at the page table level
        flags |= PagingFlags::PAT;
    entry->set(phys, flags);
}

void AddressSpace::unmap(const u64 virt)
{
    // Get the page table entry for the specified virtual address
    PageTableEntry *entry = virt2pte(virt, 0, false);

    // Get the frame mapped to the entry
    u64 frame = pte2frame(entry);
    if (!frame) // Entry not mapped
    {
        critical_dmesgln("Attempted to unmap a non-existing page %p", virt);
        return;
    }
    
    // Reset the entry & release the physical memory
    entry->set_raw(0);
    PhysicalMemoryManager::instance().release_frame(frame);
}

void *AddressSpace::virt2phys(const u64 virt)
{
    // Get the page table entry for the specified virtual address
    PageTableEntry *entry = virt2pte(virt, 0, false);

    // Get the frame mapped to the entry
    u64 frame = pte2frame(entry);
    if (!frame) // Entry not mapped
        return nullptr;
        
    return reinterpret_cast<void *>(frame | (virt & (PAGE_SIZE - 1)));
}

u64 AddressSpace::fix_flags(const u64 flags, bool& is_pat)
{
    // User does not have to specify present as when mapping, it has to be present
    u64 new_flags = flags | PagingFlags::Present;
    if (new_flags & PagingFlags::PAT)
    {
        // Disable pat because for entries > page table it means huge pages
        new_flags &= ~(PagingFlags::PAT);
        is_pat = true;
    }
    else
        is_pat = false;

    return new_flags;
}

PageTableEntry *AddressSpace::virt2pte(const u64 virt, const u64 flags, const bool allocate)
{
    // Get the page directory pointer
    PageTable *pdp = m_pml4->get_next_table(pml4_index(virt), flags, allocate);
    if (!pdp)
        return nullptr;

    // Get the page directory
    PageTable *pd = pdp->get_next_table(pdp_index(virt), flags, allocate);
    if (!pd)
        return nullptr;
    
    // Get the page table
    PageTable *pt = pd->get_next_table(pd_index(virt), flags, allocate);
    if (!pt)
        return nullptr;
    
    // Get the page table entry
    return pt->at(pt_index(virt));
}

u64 AddressSpace::pte2frame(const PageTableEntry *entry)
{
    if (!entry || !entry->is_present())
        return 0;
    
    return entry->get_frame();
}
}