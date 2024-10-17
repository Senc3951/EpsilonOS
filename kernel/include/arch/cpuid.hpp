#pragma once

#include <kernel.hpp>

namespace kernel
{
    enum CpuidFlags
    {
        EBXSMEP     = 1 << 7,
        EBXSMAP     = 1 << 20,
        ECXUMIP     = 1 << 2,
        ECXXSAVE    = 1 << 26,
        ECXAVX      = 1 << 28,
        EDXPGE      = 1 << 13,
        EDXPAT      = 1 << 16,
        EDXSSE      = 1 << 25
    };
    
    class CPUID
    {
    private:
        u32 m_eax, m_ebx, m_ecx, m_edx;
    public:
        CPUID(const u32 leaf, const u32 ecx = 0)
        {
            asm volatile("cpuid"
                            : "=a"(m_eax), "=b"(m_ebx), "=c"(m_ecx), "=d"(m_edx)
                            : "a"(leaf), "c"(ecx));
        }

        constexpr u32 eax() const { return m_eax; }
        constexpr u32 ebx() const { return m_ebx; }
        constexpr u32 ecx() const { return m_ecx; }
        constexpr u32 edx() const { return m_edx; }
    };
}