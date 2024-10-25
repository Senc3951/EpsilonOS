#pragma once

#include <arch/interrupt_handler.hpp>

namespace kernel::memory
{
    class PageFaultHandler final : public arch::GenericInterruptHandler
    {
    public:
        virtual void handle(arch::InterruptFrame *) override;
        virtual void eoi() override { }
    };
}