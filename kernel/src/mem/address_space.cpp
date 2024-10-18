#include <mem/address_space.hpp>
#include <mem/pmm.hpp>
#include <arch/register.hpp>
#include <log.hpp>

namespace kernel::memory
{
AddressSpace::AddressSpace()
{
    // Allocate memory for the table
    PhysicalAddress pml4_frame = PhysicalMemoryManager::instance().allocate_frame();
    if (pml4_frame.is_null())
        panic("failed allocating memory for pml4");
    
    // Reset the table
    m_pml4 = reinterpret_cast<PageTable *>(tohh(pml4_frame.addr()));
    m_pml4->reset();
    
    // Map lower 4GiB
    for (uintptr_t phys_addr = 0; phys_addr < 4 * GiB; phys_addr += PAGE_SIZE)
        map(tohh(phys_addr), PhysicalAddress(phys_addr), PagingFlags::Writable);
    
    // Map the kernel
    u64 kernel_size = kernel_file_request.response->kernel_file->size;
    u64 kernel_phys_start = kernel_address_request.response->physical_base;
    u64 kernel_virt_start = kernel_address_request.response->virtual_base;
    for (uintptr_t i = 0; i < kernel_size; i += PAGE_SIZE)
    {
        uintptr_t phys = kernel_phys_start + i;
        uintptr_t virt = kernel_virt_start + i;
        map(virt, PhysicalAddress(phys), PagingFlags::Writable);
    }
    
    // Map framebuffer
    limine_memmap_entry *fb_entry = PhysicalMemoryManager::find_physical_entry([](limine_memmap_entry *entry) {
        return entry->type == LIMINE_MEMMAP_FRAMEBUFFER;
    });
    u64 fb_start = tohh(fb_entry->base);
    u64 fb_size = fb_entry->length;
    for (uintptr_t i = 0; i < fb_size; i += PAGE_SIZE)
        map(fb_start + i, PhysicalAddress(fromhh(fb_start + i)), PagingFlags::Writable | PagingFlags::ExecuteDisable | WC_CACHE);
}

void AddressSpace::load()
{
    // CR3 expects the physical address of pml4
    assert(m_pml4 && "null pml4");
    Register::write(CR3, reinterpret_cast<u64>(fromhh(m_pml4)));
}

void AddressSpace::map(const u64 virt, const PhysicalAddress& phys, const u64 unfixed_flags)
{
    bool is_pat;
    u64 flags = fix_flags(unfixed_flags, is_pat);
    
    // Get the page table entry for the specified virtual address
    PageTableEntry *entry = virt2pte(virt, flags, true);
    
    if (is_pat) // Re-enable pat at the page table level
        flags |= PagingFlags::PAT;
    entry->set(phys, flags);
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
}