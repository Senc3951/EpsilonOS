#include <mem/page_fault.hpp>
#include <log.hpp>

namespace kernel::memory
{
    void PageFaultHandler::handle(arch::InterruptFrame *)
    {
        panic("page fault");
    }
}