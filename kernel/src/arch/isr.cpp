#include <arch/isr.hpp>

namespace kernel::arch
{
    GenericInterruptHandler *InterruptManager::m_interrupts[IDT_ENTRIES] = { nullptr };
    
    extern "C" InterruptFrame *isr_interrupt_handler(InterruptFrame *frame)
    {
        GenericInterruptHandler *handler = InterruptManager::at(frame->num);
        if (!handler)
            panic("unhandled interrupt %u", frame->num);
        
        handler->handle(frame);
        handler->eoi();
        
        return frame;
    }
}