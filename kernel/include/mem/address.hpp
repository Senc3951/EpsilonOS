#pragma once

#include <kernel.hpp>

namespace kernel
{
    class Address
    {
    private:
        uintptr_t m_addr;
        bool m_null = false;
    public:
        Address() : m_addr(0), m_null(true) { }
        
        template <typename T>
        Address(T addr)
        {
            m_addr = (uintptr_t)addr;
        }
        
        constexpr bool is_page_aligned() const { return (m_addr & (PAGE_SIZE - 1)) == 0; }
        constexpr bool is_null() const { return m_null; }
        
        constexpr uintptr_t addr() const { return m_addr; }
        constexpr uintptr_t page_round_up() const { return round_up(m_addr, PAGE_SIZE); }
        constexpr uintptr_t page_round_down() const { return round_down(m_addr, PAGE_SIZE); }
        
        template <typename T>
        constexpr T as() const { return reinterpret_cast<T>(m_addr); }
        
        __always_inline__ Address tohh() const { return Address(m_addr + hhdm_request.response->offset); }
        __always_inline__ Address fromhh() const { return Address(m_addr - hhdm_request.response->offset); }

        template <typename T, typename K>
        static __always_inline__ T tohh(K addr) { return (T)((K)addr + hhdm_request.response->offset); }
        
        template <typename T, typename K>
        static __always_inline__ T fromhh(K addr) { return (T)((K)addr - hhdm_request.response->offset); }
        
        constexpr bool operator==(const Address& other)
        {
            return m_addr == other.m_addr;
        }

        constexpr bool operator!=(const Address& other)
        {
            return m_addr != other.m_addr;
        }

        constexpr Address& operator+=(const u64 addr)
        {
            m_addr += addr;
            return *this;
        }
        
        friend constexpr bool operator<(const Address& a1, const Address& a2);
        friend constexpr bool operator>(const Address& a1, const Address& a2);
        friend constexpr bool operator<=(const Address& a1, const Address& a2);
        friend constexpr bool operator>=(const Address& a1, const Address& a2);
    };
    
    constexpr bool operator<(const Address& a1, const Address& a2)
    {
        return a1.m_addr < a2.m_addr;
    }

    constexpr bool operator>(const Address& a1, const Address& a2)
    {
        return a1.m_addr > a2.m_addr;
    }
    
    constexpr bool operator<=(const Address& a1, const Address& a2)
    {
        return a1.m_addr <= a2.m_addr;
    }

    constexpr bool operator>=(const Address& a1, const Address& a2)
    {
        return a1.m_addr >= a2.m_addr;
    }
}