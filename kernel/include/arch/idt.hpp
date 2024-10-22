#pragma once

#include <arch/cpu.hpp>

namespace kernel::arch
{
    constexpr size_t IDT_ENTRIES = 256;
    
    void idt_init(CPU& cpu);
}