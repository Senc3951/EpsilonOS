#pragma once

#include <lib/bitmap.hpp>
#include <mem/address.hpp>

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

        template <typename Func>
        static limine_memmap_entry *find_physical_entry(Func func, bool throw_if_not_found = true);
        
        // Delete the copy constructor & assignment operator
        PhysicalMemoryManager(const PhysicalMemoryManager&) = delete;
        PhysicalMemoryManager& operator=(const PhysicalMemoryManager&) = delete;
    };

    template <typename Func>
    limine_memmap_entry *PhysicalMemoryManager::find_physical_entry(Func func, bool throw_if_not_found)
    {
        for (uint64_t i = 0; i < memmap_request.response->entry_count; i++)
        {
            limine_memmap_entry *entry = memmap_request.response->entries[i];
            if (func(entry))
                return entry;
        }
        
        if (throw_if_not_found)
            panic("failed finding physical entry with lambda function at %p", func);
        
        return nullptr;
    }
}