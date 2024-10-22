#pragma once

#include <arch/cpu.hpp>

namespace kernel::arch
{
    void gdt_init(CPU& cpu);
}