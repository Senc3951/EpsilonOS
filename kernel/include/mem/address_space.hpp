#pragma once

#include <mem/page_table.hpp>

namespace kernel::memory
{
    class AddressSpace
    {
    private:
        PageTable *m_pml4;
        
        constexpr u64 pml4_index(const u64 x) { return (x >> 39) & 0x1FF; }
        constexpr u64 pdp_index(const u64 x) { return (x >> 30) & 0x1FF; }
        constexpr u64 pd_index(const u64 x) { return (x >> 21) & 0x1FF; }
        constexpr u64 pt_index(const u64 x) { return (x >> 12) & 0x1FF; }
        
        u64 fix_flags(const u64 flags, bool& is_pat);

        PageTableEntry *virt2pte(const u64 virt, const u64 flags, const bool allocate);
        u64 pte2frame(const PageTableEntry *entry);
    public:
        AddressSpace();

        void load();
        void flush_tlb(const u64 virt);

        void map(const u64 virt, const u64 phys, const u64 flags);
        void unmap(const u64 virt);

        void *virt2phys(const u64 virt);
    };
}