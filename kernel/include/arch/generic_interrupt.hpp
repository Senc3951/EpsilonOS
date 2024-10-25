#pragma once

#include <arch/interrupt.hpp>

namespace kernel::arch
{
    class GenericInterrupt
    {
    public:
        GenericInterrupt() { }
        virtual ~GenericInterrupt() = default;
        
        virtual void handle(InterruptFrame *) = 0;
        virtual void eoi() = 0;
    };
}