#pragma once

#include <lib/bitmap.hpp>
#include <mem/physical_address.hpp>

namespace kernel::memory
{
    constexpr size_t FRAME_SIZE = 4096;

    class PhysicalMemoryManager
    {
    private:
        static PhysicalMemoryManager m_instance;

        Bitmap<u64> m_bitmap;
        uintptr_t m_start = UINTPTR_MAX, m_end = 0;
        
        PhysicalMemoryManager() { }

        void free_region(limine_memmap_entry *entry);
    public:
        constexpr static PhysicalMemoryManager& instance() { return m_instance; }
        
        void init();

        PhysicalAddress allocate_frame();
        void release_frame(PhysicalAddress& addr);
        
        // Delete the copy constructor & assignment operator
        PhysicalMemoryManager(const PhysicalMemoryManager&) = delete;
        PhysicalMemoryManager& operator=(const PhysicalMemoryManager&) = delete;
    };
}