#include <arch/interrupt.hpp>
#include <arch/lapic.hpp>
#include <log.hpp>
#include <arch/cpu.hpp>

namespace kernel::arch
{
    extern "C" InterruptFrame *isr_interrupt_handler(InterruptFrame *frame)
    {
        switch (frame->num)
        {
        case Timer:
            dmesgln("tick");
            APIC::eoi();

            break;
        case IPIAbort:
            dmesgln("Received abort");
            
            CPU::disable_interrupts();
            CPU::hnr();
        case ApicSpurious:
            dmesgln("Spurious interrupt at %llx:%p", frame->cs, frame->rip);
            APIC::eoi();
            
            break;
        default:
            panic("Unhandled interrupt %u", frame->num);
        }
        
        return frame;
    }
}