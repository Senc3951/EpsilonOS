#pragma once

#include <kernel.hpp>

namespace kernel
{
    class CPU;
    
    constexpr u32 MAX_CPU = 64;

    struct PerCPU
    {
        uintptr_t self;
        CPU *cpu;
    };

    PerCPU *current_cpu();
}