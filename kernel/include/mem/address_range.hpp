#pragma once

#include <kernel.hpp>

namespace kernel::memory
{
    class AddressRange
    {
    public:
        AddressRange(const u64 virt_start, const u64 phys_start, const u64 size) :
            m_virt_start(virt_start), m_phys_start(phys_start), m_size(size) { }

        class Iterator
        {
        private:
            u64 m_current;
        public:
            Iterator(const u64 addr) : m_current(addr) { }

            u64 operator*() const { return m_current; }
            
            Iterator& operator++()
            {
                m_current += PAGE_SIZE;
                return *this;
            }

            bool operator!=(const Iterator& other) const { return m_current < other.m_current; }
        };

        Iterator begin() const { return Iterator(0); }
        Iterator end() const { return Iterator(m_size); }

        constexpr u64 virt_start() const { return m_virt_start; }
        constexpr u64 phys_start() const { return m_phys_start; }
        constexpr u64 size() const { return m_size; }
    private:
        u64 m_virt_start, m_phys_start, m_size;
    };
}