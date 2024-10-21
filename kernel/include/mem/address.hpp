#pragma once

#include <kernel.hpp>

namespace kernel
{
    class Address
    {
    public:
        template <typename T>
        Address(T addr)
        {
            m_addr = reinterpret_cast<uintptr_t>(addr);
        }

        constexpr bool is_page_aligned() const { return m_addr & (PAGE_SIZE - 1) == 0; }
        constexpr bool is_null() const { return m_addr == 0; }

        constexpr uintptr_t addr() const { return m_addr; }
        constexpr uintptr_t page_round_up() const { return round_up(m_addr, PAGE_SIZE); }
        constexpr uintptr_t page_round_down() const { return round_down(m_addr, PAGE_SIZE); }
        
        template <typename T>
        constexpr T addr_as() const { return reinterpret_cast<T>(m_addr); }
        
        __always_inline__ Address tohh() const { return Address(m_addr + hhdm_request.response->offset); }
        __always_inline__ Address fromhh() const { return Address(m_addr - hhdm_request.response->offset); }

        constexpr bool operator==(const Address& other)
        {
            return m_addr == other.m_addr;
        }

        constexpr bool operator!=(const Address& other)
        {
            return m_addr != other.m_addr;
        }
    private:
        uintptr_t m_addr;
    };
}