#include <arch/interrupt.hpp>
#include <log.hpp>

namespace kernel::arch
{
    extern "C" InterruptFrame *isr_interrupt_handler(InterruptFrame *frame)
    {
        // Get the interrupt handler
        u64 inum = frame->num;
        switch (inum)
        {
        default:
            panic("Unhandled interrupt %u", inum);
        }
        
        return frame;
    }
}