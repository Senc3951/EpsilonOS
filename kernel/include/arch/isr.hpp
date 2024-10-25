#pragma once

#include <arch/interrupt_handler.hpp>
#include <arch/idt.hpp>
#include <log.hpp>

namespace kernel::arch
{
    class InterruptManager
    {
    private:
        static GenericInterruptHandler *m_interrupts[IDT_ENTRIES];
    public:
        template <typename T>
        static void register_interrupt(const u8 num)
        {
            GenericInterruptHandler *old = at(num);
            if (old)
            {
                critical_dmesgln("Overwriting handler for interrupt %x", num);
                delete old;
            }
            
            m_interrupts[num] = new T();
            dmesgln("Registered a handler for interrupt %x", num);
        }
        
        static inline GenericInterruptHandler *at(const u8 num) { return m_interrupts[num]; }
    };
}