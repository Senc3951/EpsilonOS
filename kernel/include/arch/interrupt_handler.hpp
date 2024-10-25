#pragma once

#include <arch/interrupt.hpp>
#include <arch/lapic.hpp>

namespace kernel::arch
{
    class GenericInterruptHandler
    {
    public:
        GenericInterruptHandler() { }
        virtual ~GenericInterruptHandler() = default;
        
        virtual void handle(InterruptFrame *) = 0;
        virtual void eoi() = 0;
    };
    
    class IRQHandler : public GenericInterruptHandler
    {
    public:
        virtual void handle(InterruptFrame *frame) = 0;
        virtual void eoi() override { APIC::eoi(); }
    };

    // Implement some basic overrides as there is no reason to create a whole file just for them
    
    class AbortIPIHandler final : public GenericInterruptHandler
    {
    public:
        virtual void handle(InterruptFrame *) override;
        virtual void eoi() override { }
    };
}