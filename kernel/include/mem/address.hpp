#pragma once

#include <kernel.hpp>

namespace kernel::memory
{
    class Address
    {
    protected:
        uintptr_t m_addr;
        
        Address(const uintptr_t addr) : m_addr(addr) { }
    public:
        constexpr uintptr_t addr() const { return m_addr; }
        constexpr u8 *as_ptr() const { return reinterpret_cast<u8 *>(m_addr); }
        
        constexpr bool is_null() const { return m_addr == 0; }
    };

    class PhysicalAddress : public Address
    {
    public:
        PhysicalAddress(const uintptr_t addr) : Address(addr) { }
        ~PhysicalAddress() { }
        
        bool is_frame_aligned() const;
    };

    class VirtualAddress : public Address
    {
    public:
        VirtualAddress(const uintptr_t addr) : Address(addr) { }
        ~VirtualAddress() { }
        
        bool is_page_aligned() const;
    };
}