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
        
        m_frequency = 1000000000000 / (m_regs->capabilities >> 32);
        dmesgln("HPET at %p with a frequency of %llx", m_regs, m_frequency);
    }

    void HPET::msleep(const u64 ms)
    {
        volatile size_t ticks = m_regs->counter_value + (ms * m_frequency);   
        while (m_regs->counter_value < ticks)
            __pause();
    }
}