#pragma once

#include <kernel.hpp>
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
        
        void flush_tlb(const u64 virt) const;

        static void enable_interrupts() { asm volatile("sti" ::: "memory"); }
        static void disable_interrupts() { asm volatile("cli" ::: "memory"); }
        static void halt() { asm volatile("hlt" ::: "memory"); }
        static void pause() { asm volatile("pause" ::: "memory"); }
        static void __no_return__ hnr() {
            disable_interrupts();
            while (true)
                halt();
        }
        
        static __always_inline__ CPU *current()
        {
            MSR msr(MSR_IA32_GS);
            return (CPU*)msr.read();
        }
    };
}