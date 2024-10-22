#pragma once

#include <arch/cpu.hpp>

namespace kernel::arch
{
    void idt_init(CPU& cpu);
}