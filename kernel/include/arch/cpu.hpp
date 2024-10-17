#pragma once

#include <kernel.hpp>

namespace kernel
{
    class CPU
    {
    private:
        void sse_enable();
    public:
        void setup();

        static void enable_interrupts() { asm volatile("sti" ::: "memory"); }
        static void disable_interrupts() { asm volatile("cli" ::: "memory"); }
        static void halt() { asm volatile("hlt" ::: "memory"); }
        static void pause() { asm volatile("pause" ::: "memory"); }
        static void __no_return__ hnr() {
            disable_interrupts();
            while (true)
                halt();
        }
        
        static CPU *current() { return nullptr; }
    };
}