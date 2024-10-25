#include <dev/hal.hpp>
#include <dev/timer/hpet.hpp>
#include <dev/timer/apic.hpp>
#include <dev/timer/interrupt.hpp>
#include <arch/isr.hpp>

namespace kernel::dev
{
    using namespace arch;
    using namespace timer;

    void HAL::init()
    {
        /* Initialize timers */
        // First initialize the HPET as the APIC timer relies on it
        HPET::instance().init();

        // Initialize other devices
        APICTimer::init();
        
        // Register interrupt handler for timer
        InterruptManager::register_interrupt<TimerInterruptHandler>(Timer);
    }

    void HAL::msleep(const u64 ms)
    {
        APICTimer::msleep(ms);
    }

    void HAL::oneshot(const u64 ms)
    {
        APICTimer::oneshot(ms);
    }

    void HAL::periodic(const u64 ms)
    {
        APICTimer::periodic(ms);
    }
}