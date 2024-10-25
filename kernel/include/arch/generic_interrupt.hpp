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

    // Implement some basic overrides as there is no reason to create a whole file just for them
    
    class ApicSpuriousInterruptHandler final : public GenericInterrupt
    {
    public:
        virtual void handle(InterruptFrame *) override;
        virtual void eoi() override;
    };
    
    class AbortIPIHandler final : public GenericInterrupt
    {
    public:
        virtual void handle(InterruptFrame *) override;
        virtual void eoi() override { }
    };
}