#pragma once

#include <kernel.hpp>

namespace kernel
{
    class Address
    {
    protected:
        uintptr_t m_addr, m_aligned_addr;
        
        Address(const uintptr_t addr, const size_t align) : m_addr(addr)
        {
            m_aligned_addr = round_down(m_addr, align);
        }
    public:
        constexpr uintptr_t addr() const { return m_addr; }
        constexpr uintptr_t aligned_addr() const { return m_aligned_addr; }
        
        constexpr bool is_null() const { return m_addr == 0; }
    };
}