#pragma once

#include <kernel.hpp>

namespace kernel
{
    constexpr u32 MSR_IA32_PAT = 0x277;

    class MSR
    {
    private:
        u32 m_msr;
    public:
        MSR(const u32 msr) : m_msr(msr) { }

        u64 get() const
        {
            u32 low, high;
            asm volatile("rdmsr"
                        : "=a"(low), "=d"(high)
                        : "c"(m_msr));
            return ((u64)high << 32) | low;
        }

        void set(const u64 value) const
        {
            u32 low = static_cast<u32>(value);
            u32 high = static_cast<u32>(value >> 32);
            
            asm volatile("wrmsr" ::"a"(low), "d"(high), "c"(m_msr));
        }
    };
}