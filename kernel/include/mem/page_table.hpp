#pragma once

#include <kernel.hpp>
#include <mem/address.hpp>

namespace kernel::memory
{
    constexpr size_t PAGE_SIZE = 4096;
    constexpr size_t ENTRIES_PER_TABLE = 512;

    enum PagingFlags
    {
        Present         = 1 << 0,
        Writable        = 1 << 1,
        UserAccessible  = 1 << 2,
        WriteThrough    = 1 << 3,
        CacheDisable    = 1 << 4,
        PAT             = 1 << 7,
        ExecuteDisable  = (1ull << 63)
    };
    
    constexpr u64 WC_CACHE = PagingFlags::PAT;

    class PageTableEntry
    {
    private:
        u64 m_raw;
        #define FLAGS_MASK 0xFFF0000000000FFF        
    public:
        constexpr bool is_null() const { return m_raw == 0; }
        constexpr bool is_present() const { return m_raw & PagingFlags::Present; }
        
        PhysicalAddress get_frame() const { return PhysicalAddress(m_raw & ~FLAGS_MASK); }
        
        void set(const PhysicalAddress& frame, const u64 flags)
        {
            m_raw = flags & FLAGS_MASK;
            m_raw |= frame.addr() & ~FLAGS_MASK;
        }
    };
    
    class PageTable
    {
    private:
        PageTableEntry m_entries[ENTRIES_PER_TABLE];

        PageTable *create_table(PageTableEntry *entry, const u64 flags);
    public:
        void reset();
        PageTableEntry *at(const u32 i) { return &m_entries[i]; }
        
        PageTable *get_next_table(const size_t index, const u64 flags, const bool allocate);
    };

    static_assert(sizeof(PageTableEntry) == 8);
    static_assert(sizeof(PageTable) == PAGE_SIZE);
}