#include <arch/isr.hpp>

namespace kernel::arch
{
    GenericInterrupt *InterruptManager::m_interrupts[IDT_ENTRIES] = { nullptr };
    
    extern "C" InterruptFrame *isr_interrupt_handler(InterruptFrame *frame)
    {
        GenericInterrupt *interrupt = InterruptManager::at(frame->num);
        if (!interrupt)
            panic("unhandled interrupt %u", frame->num);
        
        interrupt->handle(frame);
        interrupt->eoi();

        return frame;
    }
}