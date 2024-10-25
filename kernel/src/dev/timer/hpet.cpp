#include <dev/timer/hpet.hpp>
#include <acpi/rsdt.hpp>
#include <mem/address.hpp>
#include <log.hpp>

namespace kernel::dev::timer
{
    #define RSDT_HPET_NAME "HPET"

    HPET HPET::m_instance;
    
    void HPET::init()
    {
        m_hpet = reinterpret_cast<HpetTable *>(acpi::RSDT::instance().find_table(RSDT_HPET_NAME));
        assert(m_hpet && "hpet not found");
        
        // Enable the timer & reset the counter
        m_regs = Address::tohh<HpetRegs *, u64>(m_hpet->address);
        m_regs->counter_value = 0;
        m_regs->general_config = 1;
        
        dmesgln("HPET at %p", m_regs);
    }

    void HPET::msleep(const u64 ms)
    {
        u32 period = m_regs->capabilities >> 32;
        volatile size_t ticks = m_regs->counter_value + (ms * (1000000000000 / period));
        
        while (m_regs->counter_value < ticks)
            asm volatile("pause");
    }
}