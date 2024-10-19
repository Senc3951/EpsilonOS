#include <mem/address_space.hpp>
#include <mem/pmm.hpp>
#include <arch/register.hpp>
#include <arch/cpu.hpp>
#include <log.hpp>

namespace kernel::memory
{
    AddressSpace kernel_address_space;

    AddressSpace::AddressSpace() { }
    
    void  AddressSpace::init_kernel()
    {
        // Allocate memory for the table
        void *pml4_frame = PhysicalMemoryManager::instance().allocate_frame();
        if (!pml4_frame)
            panic("failed allocating memory for pml4");
        
        // Reset the table
        m_pml4 = reinterpret_cast<PageTable *>(tohh(pml4_frame));
        m_pml4->reset();

        // Map lower 4GiB
        AddressRange _4gib_range(tohh(static_cast<u64>(0)), 0, 4 * GiB);
        map(_4gib_range, PagingFlags::Writable | PagingFlags::ExecuteDisable);

        // Protect NULL to be not writable & not excutable
        map(tohh(static_cast<u64>(0)), 0, PagingFlags::ExecuteDisable);

        // Map the kernel
        u64 kernel_phys_start = kernel_address_request.response->physical_base;
        u64 kernel_virt_start = kernel_address_request.response->virtual_base;

        // Convert the kernel exports to addresses
        u64 requests_start = reinterpret_cast<u64>(&_requests_start);
        u64 requests_end = reinterpret_cast<u64>(&_requests_end);
        u64 text_start = reinterpret_cast<u64>(&_text_start);
        u64 text_end = reinterpret_cast<u64>(&_text_end);
        u64 rodata_start = reinterpret_cast<u64>(&_rodata_start);
        u64 rodata_end = reinterpret_cast<u64>(&_rodata_end);
        u64 ctor_start = reinterpret_cast<u64>(&_ctor_start);
        u64 ctor_end = reinterpret_cast<u64>(&_ctor_end);
        u64 data_start = reinterpret_cast<u64>(&_data_start);
        u64 data_end = reinterpret_cast<u64>(&_data_end);

        // Map requests region as Writable & Not Executable
        AddressRange kernel_requests_range(requests_start, kernel_phys_start + requests_start - kernel_virt_start, requests_end - requests_start);
        map(kernel_requests_range, PagingFlags::Writable | PagingFlags::ExecuteDisable);

        // Map text region as Not Writable & Executable
        AddressRange kernel_text_range(text_start, kernel_phys_start + text_start - kernel_virt_start, text_end - text_start);
        map(kernel_text_range, 0);

        // Map rodata region as Not Writable & Not Executable
        AddressRange kernel_rodata_range(rodata_start, kernel_phys_start + rodata_start - kernel_virt_start, rodata_end - rodata_start);
        map(kernel_rodata_range, PagingFlags::ExecuteDisable);

        // Map constructors region as Not Writable & Executable
        AddressRange kernel_ctor_region(ctor_start, kernel_phys_start + ctor_start - kernel_virt_start, ctor_end - ctor_start);
        map(kernel_ctor_region, 0);

        // Map data region as Writable & Not Executable
        AddressRange kernel_data_range(data_start, kernel_phys_start + data_start - kernel_virt_start, data_end - data_start);
        map(kernel_data_range, PagingFlags::Writable | PagingFlags::ExecuteDisable);

        // Get framebuffer physical entry
        limine_memmap_entry *framebuffer_memmap_entry = PhysicalMemoryManager::find_physical_entry([](limine_memmap_entry *entry) {
            return entry->type == LIMINE_MEMMAP_FRAMEBUFFER;
        });
        
        // Map framebuffer
        AddressRange framebuffer_range(reinterpret_cast<u64>(framebuffer_request.response->framebuffers[0]->address),
            framebuffer_memmap_entry->base, framebuffer_memmap_entry->length);
        map(framebuffer_range, PagingFlags::Writable | PagingFlags::ExecuteDisable | WC_CACHE);
    
        critical_dmesgln("Kernel PML4 at %p", fromhh(m_pml4));
        load();
    }

    void AddressSpace::load()
    {
        // CR3 expects the physical address of pml4
        assert(m_pml4 && "null pml4");
        Register::write(CR3, reinterpret_cast<u64>(fromhh(m_pml4)));
    }

    void AddressSpace::flush_tlb(const u64 virt)
    {
        CPU::current()->flush_tlb(virt);
    }

    void AddressSpace::map(const u64 virt, const u64 phys, const u64 unfixed_flags)
    {
        u64 pte_flags;
        u64 flags = fix_flags(unfixed_flags, pte_flags);
        
        // Get the page table entry for the specified virtual address
        PageTableEntry *entry = virt2pte(virt, flags, true);

        // If remapping to a different frame, release the current one
        u64 frame = pte2frame(entry);
        if (frame && frame != phys)
            PhysicalMemoryManager::instance().release_frame(frame);

        // Set page table entry with its special flags
        entry->set(phys, pte_flags);
    }

    void AddressSpace::map(AddressRange& range, const u64 flags)
    {
        u64 virt = range.virt_start();
        u64 phys = range.phys_start();
        for (auto offset : range)
            map(virt + offset, phys + offset, flags);
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

    void AddressSpace::unmap(AddressRange &range)
    {
        u64 virt = range.virt_start();
        for (auto offset : range)
            unmap(virt + offset);
    }

    void *AddressSpace::allocate(const size_t page_count, const u64 flags)
    {
        if (!page_count)
            return nullptr;

        // Allocate a frame for the first page
        void *first_frame_phys = PhysicalMemoryManager::instance().allocate_frame();
        if (!first_frame_phys)
            return nullptr;

        // Map the first page
        u64 first_frame = reinterpret_cast<u64>(tohh(first_frame_phys));
        map(first_frame, reinterpret_cast<u64>(first_frame_phys), flags);
        flush_tlb(first_frame);

        // Map the other pages
        for (size_t i = 1; i < page_count; i++)
        {
            void *frame = PhysicalMemoryManager::instance().allocate_frame();
            if (!frame)
            {
                // Unmap all allocated pages
                release(reinterpret_cast<void *>(first_frame), i);            
                return NULL;
            }
                
            // Map the other pages after the first page and flush the tlb
            u64 vaddr = first_frame + i * PAGE_SIZE;
            map(vaddr, reinterpret_cast<u64>(frame), flags);
            flush_tlb(vaddr);
        }
        
        return reinterpret_cast<void *>(first_frame);   
    }

    void AddressSpace::release(void *ptr, const size_t page_count)
    {
        u64 vbase = round_down(reinterpret_cast<u64>(ptr), PAGE_SIZE);
        for (size_t i = 0; i < page_count; i++)
        {
            u64 virt = vbase + i * PAGE_SIZE;

            // Unmap & flush each page
            unmap(virt);
            flush_tlb(virt);
        }
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

    u64 AddressSpace::fix_flags(const u64 flags, u64& pte_flags)
    {
        // User does not have to specify present as when mapping, it has to be present
        // pte_flags are the original flags with Present, returned flags are page directory flags, meaning
        // they don't contain pat & execution disable.
        u64 pd_flags = pte_flags = flags | PagingFlags::Present;

        if (pd_flags & PagingFlags::PAT)
            // Disable pat because for entries > page table it means huge pages
            pd_flags &= ~(PagingFlags::PAT);
        if (pd_flags & PagingFlags::ExecuteDisable)
            // Disable xd because it can cause undefined behaviour (only enable xd on page table level)
            pd_flags &= ~(PagingFlags::ExecuteDisable);

        return pd_flags;
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