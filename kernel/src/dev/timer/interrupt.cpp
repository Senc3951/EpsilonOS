#include <dev/timer/interrupt.hpp>
#include <log.hpp>

namespace kernel::dev::timer
{
    using namespace arch;
    
    void TimerInterruptHandler::handle(InterruptFrame *)
    {
        dmesgln("tick");
    }
}