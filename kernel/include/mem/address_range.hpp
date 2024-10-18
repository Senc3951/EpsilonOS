#pragma once

#include <kernel.hpp>

namespace kernel::memory
{
    class AddressRange
    {
    public:
        AddressRange(const u64 start, const u64 size) : m_start(start), m_end(start + size) { }

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
    
        Iterator begin() const { return Iterator(m_start); }
        Iterator end() const { return Iterator(m_end); }
    private:
        u64 m_start, m_end;
    };
}