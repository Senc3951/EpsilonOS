#include <arch/interrupt.hpp>
#include <arch/lapic.hpp>
#include <log.hpp>

namespace kernel::arch
{
    extern "C" InterruptFrame *isr_interrupt_handler(InterruptFrame *frame)
    {
        // Get the interrupt handler
        u64 inum = frame->num;
        switch (inum)
        {
        case ApicSpurious:
            dmesgln("Spurious interrupt at %llx:%p", frame->cs, frame->rip);
            APIC::eoi();
            
            break;
        default:
            panic("Unhandled interrupt %u", inum);
        }
        
        return frame;
    }
}