#pragma once

#include <mem/address.hpp>
#include <log.hpp>
namespace kernel::memory
{
    class AddressRange
    {
    public:
        AddressRange(const uintptr_t virt_start, const uintptr_t phys_start, const u64 size) :
            m_virt_start(virt_start), m_phys_start(phys_start), m_size(size) { }
        
        class Iterator
        {
        private:
            uintptr_t m_current;
        public:
            Iterator(const uintptr_t addr) : m_current(addr) { }
            
            uintptr_t operator*() const { return m_current; }
            
            Iterator& operator++()
            {
                m_current += PAGE_SIZE;
                return *this;
            }
            
            bool operator!=(const Iterator& other) const { return m_current < other.m_current; }
        };

        Iterator begin() const { return Iterator(0); }
        Iterator end() const { return Iterator(m_size); }
        
        inline uintptr_t virt_start() const { return m_virt_start; }
        inline uintptr_t phys_start() const { return m_phys_start; }
        constexpr u64 size() const { return m_size; }
    private:
        uintptr_t m_virt_start, m_phys_start;
        u64 m_size;
    };
}