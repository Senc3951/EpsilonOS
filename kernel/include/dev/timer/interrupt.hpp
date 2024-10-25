#pragma once

#include <arch/generic_interrupt.hpp>

namespace kernel::dev::timer
{
    class TimerInterruptHandler : public arch::GenericInterrupt
    {
    public:
        virtual void handle(arch::InterruptFrame *) override;
        virtual void eoi() override;
    };
}