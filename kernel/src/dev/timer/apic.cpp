#include <dev/timer/apic.hpp>
#include <dev/timer/hpet.hpp>
#include <arch/lapic.hpp>
#include <arch/ioapic.hpp>
#include <arch/interrupt.hpp>
#include <log.hpp>

namespace kernel::dev::timer
{
    using namespace arch;

    constexpr u64 CALIBRATION_MS = 10;
    constexpr u32 DIVIDER = 3;
    constexpr u32 MODE_ONESHOT = 0 << 17;
    constexpr u32 MOD_PERIODIC = 1 << 17;

    u64 APICTimer::m_ticks_per_ms;
    
    void APICTimer::init()
    {
        u32 apicid = APIC::id();

        // Configure the timer
        APIC::write(LAPIC_TDCR, DIVIDER);
        APIC::write(LAPIC_TICR, UINT32_MAX);
        
        // Sleep for a couple of milliseconds
        HPET::instance().msleep(CALIBRATION_MS);
        APIC::write(LAPIC_TIMER, LAPIC_MASKED);
        
        /**
         * Calculate how many ticks in a micro second.
         * UINT32_MAX - ticks is the elpased ticks in CALIBRATION_MS milliseconds.
         * By dividing by it, we get ticks per millisecond, and by dividing by a 1000, we get ticks
         * in a microsecond.
         */
        uint32_t ticks = APIC::read(LAPIC_TCCR);
        m_ticks_per_ms = (UINT32_MAX - ticks) / CALIBRATION_MS;
        dmesgln("Calculated %llu ticks per millisecond for apic timer on cpu %u", m_ticks_per_ms, apicid);
        
        // Map I/O interrupt to current cpu
        IOAPIC::map(toirq(Interrupt::Timer), apicid);
    }
    
    void APICTimer::msleep(const u64 ms)
    {
        // Set timer as oneshot but masked, meaning it will not generate an interrupt
        APIC::write(LAPIC_TIMER, MODE_ONESHOT | Interrupt::Timer | LAPIC_MASKED);

        // Set the counter
        set_counter(ms);

        // Wait till the timer reaches 0
        while (APIC::read(LAPIC_TCCR) != 0)
            __pause();
    }
    
    void APICTimer::oneshot(const u64 ms)
    {
        APIC::write(LAPIC_TIMER, MODE_ONESHOT | Interrupt::Timer);
        set_counter(ms);
    }

    void APICTimer::periodic(const u64 ms)
    {
        APIC::write(LAPIC_TIMER, MOD_PERIODIC | Interrupt::Timer);
        set_counter(ms);
    }

    void APICTimer::set_counter(const u64 ms)
    {
        APIC::write(LAPIC_TICR, m_ticks_per_ms * ms);
    }
}