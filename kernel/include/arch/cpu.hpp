#pragma once

#include <kernel.hpp>
#include <arch/percpu.hpp>

namespace kernel
{
    class CPU
    {
    private:
        void sse_enable();
    public:
        void init();

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
        
        static __always_inline__ CPU *current() { return current_cpu()->cpu; }
    };
}