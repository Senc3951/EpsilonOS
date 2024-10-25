#pragma once

#include <arch/generic_interrupt.hpp>

namespace kernel::memory
{
    class PageFaultHandler : public arch::GenericInterrupt
    {
    public:
        virtual void handle(arch::InterruptFrame *) override;
        virtual void eoi() override { }
    };
}