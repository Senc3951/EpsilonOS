#pragma once

#include <arch/msr.hpp>

namespace kernel
{
    constexpr u32 MAX_CPU = 64;

    class CPU
    {
    private:
        u32 m_apicid;
        static u32 m_id;

        static void sse_enable();
        static CPU *get_next_cpu();
    public:
        static CPU *init(); /* Returns a new instance of a CPU */
        static __always_inline__ CPU *current() { return reinterpret_cast<CPU *>(MSR::read(MSR_IA32_GS)); }
        
        static __always_inline__ void enable_interrupts() { asm volatile("sti" ::: "memory"); }
        static __always_inline__ void disable_interrupts() { asm volatile("cli" ::: "memory"); }
        static __always_inline__ void halt() { asm volatile("hlt" ::: "memory"); }
        static __always_inline__ void pause() { asm volatile("pause" ::: "memory"); }
        static __always_inline__ __no_return__ void hnr() {
            disable_interrupts();
            while (true)
                halt();
        }
        
        void flush_tlb(const u64 virt) const;
        
        constexpr u32 get_apicid() const { return m_apicid; }
        constexpr void set_apicid(const u32 apicid) { m_apicid = apicid; }
    };
}