#pragma once

#include <arch/interrupt_handler.hpp>

namespace kernel::dev::timer
{
    class TimerInterruptHandler final : public arch::IRQHandler
    {
    public:
        virtual void handle(arch::InterruptFrame *) override;
    };
}