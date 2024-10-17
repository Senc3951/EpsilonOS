#include <mem/pmm.hpp>
#include <lib/string.h>
#include <log.hpp>

namespace kernel::memory
{
PhysicalMemoryManager PhysicalMemoryManager::m_instance;

void PhysicalMemoryManager::init()
{
    // Find highest available address
    for (uint64_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        limine_memmap_entry *entry = memmap_request.response->entries[i];
        dmesgln("Physical Region at %p-%p (%u)", entry->base, entry->base + entry->length, entry->type);
    
        // Calculate the start & end of available physical memory
        if (entry->base < m_start)
            m_start = entry->base;
        if (entry->type == LIMINE_MEMMAP_USABLE && entry->base + entry->length > m_end)
            m_end = entry->base + entry->length;
    }
    
    // Calculate the size of the bitmap (depends on bitmap type)
    u64 bsize = round_up(round_down(m_end, FRAME_SIZE) / FRAME_SIZE, 64) / 8;

    // Find a place to store the bitmap
    for (uint64_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        limine_memmap_entry *entry = memmap_request.response->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= bsize)
        {
            // Initialize the bitmap
            m_bitmap.init(reinterpret_cast<u64 *>(entry->base + hhdm_request.response->offset), UINT64_MAX, bsize);
            
            // Update the entry
            entry->length -= bsize;
            entry->base += bsize;
            break;
        }
    }

    // Verify found a place to store the bitmap
    assert(!m_bitmap.is_null() && "failed to find a place to store the bitmap");

    // Mark all memory as used
    memset(reinterpret_cast<void *>(m_bitmap.addr()), 0xFF, m_bitmap.bsize());
    
    // Free usable regions
    for (uint64_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        limine_memmap_entry *entry = memmap_request.response->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE)
            free_region(entry);
    }
    
    critical_dmesgln("Physical Memory bitmap at %p (%llu bytes)\n", m_bitmap.addr(), m_bitmap.bsize());
}

PhysicalAddress PhysicalMemoryManager::allocate_frame()
{
    // Find an available bit
    s64 num = m_bitmap.find_set();
    if (num < 0)
        return PhysicalAddress(0);
    
    return PhysicalAddress(num * FRAME_SIZE);
}

void PhysicalMemoryManager::release_frame(PhysicalAddress& addr)
{
    // Check that the address is aligned
    if (!addr.is_frame_aligned())
        panic("unaligned physical address %p", addr.addr());

    // Free the bit
    m_bitmap.clear(addr.addr() / FRAME_SIZE, true);
}

void PhysicalMemoryManager::free_region(limine_memmap_entry *entry)
{
    u64 frame_count = round_down(entry->length, FRAME_SIZE) / FRAME_SIZE;
    u64 start_frame = round_up(entry->base, FRAME_SIZE) / FRAME_SIZE;
    for (u64 i = 0; i < frame_count; i++)
        m_bitmap.clear(start_frame + i, true);
}
}