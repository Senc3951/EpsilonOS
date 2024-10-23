#pragma once

#include <kernel.hpp>

namespace kernel
{
    constexpr u32 MSR_IA32_APIC     = 0x1B;
    constexpr u32 MSR_IA32_PAT      = 0x277;
    constexpr u32 MSR_IA32_FS       = 0xC0000100;
    constexpr u32 MSR_IA32_GS       = 0xC0000101;
    constexpr u32 MSR_IA32_KERNELGS = 0xC0000102;    

    class MSR
    {
    private:
        u32 m_msr;
    public:
        MSR(const u32 msr) : m_msr(msr) { }
        
        inline u64 read() const { return read(m_msr); }
        inline void write(const u64 value) const { write(m_msr, value); }

        static inline u64 read(const u32 msr)
        {
            u32 low, high;
            asm volatile("rdmsr"
                        : "=a"(low), "=d"(high)
                        : "c"(msr));
            return ((u64)high << 32) | low;   
        }

        static void write(const u32 msr, const u64 value)
        {
            u32 low = static_cast<u32>(value);
            u32 high = static_cast<u32>(value >> 32);
            
            asm volatile("wrmsr" ::"a"(low), "d"(high), "c"(msr));
        }
    };
}