#include <mem/address_space.hpp>
#include <mem/pmm.hpp>
#include <mem/page_fault.hpp>
#include <arch/register.hpp>
#include <arch/isr.hpp>
#include <log.hpp>

namespace kernel::memory
{
    AddressSpace kernel_address_space;

    AddressSpace::AddressSpace() { }
    
    void AddressSpace::init_kernel()
    {
        // Allocate memory for the table
        m_phys_pml4 = PhysicalMemoryManager::instance().allocate_frame();
        if (m_phys_pml4.is_null() || !m_phys_pml4.is_page_aligned())
            panic("failed allocating memory for pml4");
        
        // Reset the table
        m_pml4 = m_phys_pml4.tohh().as<PageTable *>();
        m_pml4->reset();

        // Map lower 4GiB as Writable & Not Executable
        AddressRange _4gib_range(Address::tohh<u64, u64>(0), 0, 4 * GiB);
        map(_4gib_range, PagingFlags::Writable | PagingFlags::ExecuteDisable);
        
        // Map NULL to be Not Writable & Not Executable
        map(Address::tohh<u64, u64>(0), 0, PagingFlags::ExecuteDisable);
        
        // Convert the kernel exports to addresses
        u64 kernel_phys_start = kernel_address_request.response->physical_base;
        u64 kernel_virt_start = kernel_address_request.response->virtual_base;
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
        
        // Map the Kernel
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
        
        // Map the framebuffer as Writable, Not Executable & WC cached
        AddressRange framebuffer_range(reinterpret_cast<u64>(framebuffer_request.response->framebuffers[0]->address),
            framebuffer_memmap_entry->base, framebuffer_memmap_entry->length);
        map(framebuffer_range, PagingFlags::Writable | PagingFlags::ExecuteDisable | WC_CACHE);
    
        critical_dmesgln("Kernel PML4 at %p", m_phys_pml4.addr());
        load();
        
        // Register an interrupt handler for page fault
        // This will initialize the heap
        arch::InterruptManager::register_interrupt<PageFaultHandler>(arch::Interrupt::PageFault);
    }

    void AddressSpace::load()
    {
        // CR3 expects the physical address of pml4
        assert(!m_phys_pml4.is_null() && "null pml4");
        Register::write(CR3, m_phys_pml4.addr());
    }
    
    void AddressSpace::flush_tlb(const Address& virt)
    {
        uintptr_t addr = virt.addr();
        asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
    }

    void AddressSpace::map(const Address& virt, const Address& phys, const u64 unfixed_flags)
    {
        u64 pte_flags;
        u64 flags = fix_flags(unfixed_flags, pte_flags);
        
        // Get the page table entry for the specified virtual address
        PageTableEntry *entry = virt2pte(virt, flags, true);

        // If remapping to a different frame, release the current one
        Address frame = pte2frame(entry);
        if (!frame.is_null() && frame != phys)
            PhysicalMemoryManager::instance().release_frame(frame);

        // Set page table entry with its special flags
        entry->set(phys, pte_flags);
    }

    void AddressSpace::map(AddressRange& range, const u64 flags)
    {
        uintptr_t virt = range.virt_start().addr();
        uintptr_t phys = range.phys_start().addr();
        for (auto offset : range)
        {
            Address vaddr(virt + offset.addr());
            Address paddr(phys + offset.addr());
            
            map(vaddr, paddr, flags);
        }
    }
    
    void AddressSpace::unmap(const Address& virt)
    {
        // Get the page table entry for the specified virtual address
        PageTableEntry *entry = virt2pte(virt, 0, false);

        // Get the frame mapped to the entry
        Address frame = pte2frame(entry);
        if (frame.is_null()) // Entry not mapped
        {
            critical_dmesgln("Attempted to unmap a non-existing page %p", virt);
            return;
        }
        
        // Reset the entry & release the physical memory
        entry->set_raw(0);
        PhysicalMemoryManager::instance().release_frame(frame);
    }

    void AddressSpace::unmap(AddressRange& range)
    {
        uintptr_t virt = range.virt_start().addr();
        for (auto offset : range)
        {
            Address vaddr(virt + offset.addr());
            unmap(vaddr);
        }
    }

    Address AddressSpace::allocate(const size_t page_count, const u64 flags)
    {
        if (!page_count)
            return Address();

        // Allocate a frame for the first page
        Address first_frame_phys = PhysicalMemoryManager::instance().allocate_frame();
        if (first_frame_phys.is_null())
            return Address();
        
        // Map the first page
        Address first_frame = first_frame_phys.tohh();
        map(first_frame, first_frame_phys, flags);
        flush_tlb(first_frame);

        // Map the other pages
        for (size_t i = 1; i < page_count; i++)
        {
            Address frame = PhysicalMemoryManager::instance().allocate_frame();
            if (frame.is_null())
            {
                // Unmap all allocated pages
                release(first_frame, i);            
                return Address();
            }
                
            // Map the other pages after the first page and flush the tlb
            Address vaddr(first_frame.addr() + i * PAGE_SIZE);
            map(vaddr, frame, flags);
            flush_tlb(vaddr);
        }
        
        return first_frame;   
    }

    void AddressSpace::release(const Address& virt, const size_t page_count)
    {
        uintptr_t vbase = virt.page_round_down();
        for (size_t i = 0; i < page_count; i++)
        {
            uintptr_t vaddr = vbase + i * PAGE_SIZE;
            
            // Unmap & flush each page
            unmap(vaddr);
            flush_tlb(vaddr);
        }
    }

    Address AddressSpace::virt2phys(const Address& virt)
    {
        // Get the page table entry for the specified virtual address
        uintptr_t vaddr = virt.addr();
        PageTableEntry *entry = virt2pte(vaddr, 0, false);

        // Get the frame mapped to the entry
        Address frame = pte2frame(entry);
        if (frame.is_null()) // Entry not mapped
            return Address();
            
        return Address(frame.addr() | (vaddr & (PAGE_SIZE - 1)));
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

    PageTableEntry *AddressSpace::virt2pte(const Address& virt, const u64 flags, const bool allocate)
    {
        // Get the page directory pointer
        uintptr_t vaddr = virt.addr();
        PageTable *pdp = m_pml4->get_next_table(pml4_index(vaddr), flags, allocate);
        if (!pdp)
            return nullptr;

        // Get the page directory
        PageTable *pd = pdp->get_next_table(pdp_index(vaddr), flags, allocate);
        if (!pd)
            return nullptr;
        
        // Get the page table
        PageTable *pt = pd->get_next_table(pd_index(vaddr), flags, allocate);
        if (!pt)
            return nullptr;
        
        // Get the page table entry
        return pt->at(pt_index(vaddr));
    }

    Address AddressSpace::pte2frame(const PageTableEntry *entry)
    {
        if (!entry || !entry->is_present())
            return Address();
        
        return entry->get_frame();
    }
}