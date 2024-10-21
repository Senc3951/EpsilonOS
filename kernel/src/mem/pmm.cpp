#include <mem/pmm.hpp>
#include <lib/string.h>
#include <log.hpp>

namespace kernel::memory
{
    PhysicalMemoryManager PhysicalMemoryManager::m_instance;

    void PhysicalMemoryManager::init()
    {
        u64 mem_end = 0;
     
        // Find highest available address
        for (uint64_t i = 0; i < memmap_request.response->entry_count; i++)
        {
            limine_memmap_entry *entry = memmap_request.response->entries[i];
            dmesgln("Physical Region at %p-%p (%u)", entry->base, entry->base + entry->length, entry->type);
        
            // Calculate the start & end of available physical memory
            if (entry->type == LIMINE_MEMMAP_USABLE && entry->base + entry->length > mem_end)
                mem_end = entry->base + entry->length;
        }
        
        // Calculate the size of the bitmap (depends on bitmap type)
        u64 bsize = round_up(round_down(mem_end, FRAME_SIZE) / FRAME_SIZE, 64) / 8;

        // Find a place to store the bitmap
        limine_memmap_entry *bitmap_entry = find_physical_entry([&bsize](limine_memmap_entry *entry) {
            return entry->type == LIMINE_MEMMAP_USABLE && entry->length >= bsize;
        });
        
        // Initialize the bitmap & update the entry
        Address bitmap_address(bitmap_entry->base);
        m_bitmap.init(bitmap_address.tohh().as<u64 *>(), UINT64_MAX, bsize);
        bitmap_entry->length -= bsize;
        bitmap_entry->base += bsize;
        
        // Mark all memory as used
        memset(reinterpret_cast<void *>(m_bitmap.addr()), 0xFF, m_bitmap.bsize());
        
        // Free usable regions
        for (uint64_t i = 0; i < memmap_request.response->entry_count; i++)
        {
            limine_memmap_entry *entry = memmap_request.response->entries[i];
            if (entry->type == LIMINE_MEMMAP_USABLE)
                free_region(entry);
        }
        
        critical_dmesgln("Physical Memory bitmap at %p (%llu bytes)", m_bitmap.addr(), m_bitmap.bsize());
    }

    Address PhysicalMemoryManager::allocate_frame()
    {
        // Find an available bit
        s64 num = m_bitmap.find_set();
        if (num < 0)
            return Address();
        
        return Address(num * FRAME_SIZE);
    }

    void PhysicalMemoryManager::release_frame(Address& address)
    {
        // Check that the address is aligned
        if (!address.is_page_aligned())
            panic("unaligned physical address %p", address.addr());
        
        // Free the bit
        m_bitmap.clear(address.addr() / FRAME_SIZE, true);
    }

    void PhysicalMemoryManager::free_region(limine_memmap_entry *entry)
    {
        u64 frame_count = round_down(entry->length, FRAME_SIZE) / FRAME_SIZE;
        u64 start_frame = round_up(entry->base, FRAME_SIZE) / FRAME_SIZE;
        for (u64 i = 0; i < frame_count; i++)
            m_bitmap.clear(start_frame + i, true);
    }
}