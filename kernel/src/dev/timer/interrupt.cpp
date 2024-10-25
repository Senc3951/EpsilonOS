#include <dev/timer/interrupt.hpp>
#include <arch/lapic.hpp>
#include <log.hpp>

namespace kernel::dev::timer
{
    using namespace arch;

    void TimerInterruptHandler::handle(arch::InterruptFrame *)
    {
        dmesgln("tick");
    }

    void TimerInterruptHandler::eoi()
    {
        APIC::eoi();
    }
}