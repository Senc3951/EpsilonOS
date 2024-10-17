#pragma once

#include <lib/address.hpp>

namespace kernel::memory
{
    class PhysicalAddress : public Address
    {
    public:
        PhysicalAddress(const uintptr_t addr);
        ~PhysicalAddress() { }

        constexpr bool is_frame_aligned() const { return m_addr == m_aligned_addr; }
    };
}