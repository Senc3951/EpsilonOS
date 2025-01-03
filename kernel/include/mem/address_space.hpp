#pragma once

#include <mem/page_table.hpp>
#include <mem/address_range.hpp>

namespace kernel::memory
{
    class AddressSpace
    {
    private:
        Address m_phys_pml4;
        PageTable *m_pml4;
                
        constexpr u64 pml4_index(const u64 x) { return (x >> 39) & 0x1FF; }
        constexpr u64 pdp_index(const u64 x) { return (x >> 30) & 0x1FF; }
        constexpr u64 pd_index(const u64 x) { return (x >> 21) & 0x1FF; }
        constexpr u64 pt_index(const u64 x) { return (x >> 12) & 0x1FF; }
        
        u64 fix_flags(const u64 flags, u64& pte_flags);

        PageTableEntry *virt2pte(const Address& virt, const u64 flags, const bool allocate);
        Address pte2frame(const PageTableEntry *entry);
    public:
        AddressSpace();

        void init_kernel();
        void load();
        
        void map(const Address& virt, const Address& phys, const u64 flags);
        void map(AddressRange& range, const u64 flags);
        void unmap(const Address& virt);
        void unmap(AddressRange& range);
        
        Address allocate(const size_t page_count, const u64 flags);
        void release(const Address& virt, const size_t page_count);
        
        void flush_tlb(const Address& virt);
        Address virt2phys(const Address& virt);
    };

    extern AddressSpace kernel_address_space;
}