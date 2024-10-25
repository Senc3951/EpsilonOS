#include <mem/page_fault.hpp>
#include <arch/register.hpp>

namespace kernel::memory
{
    void PageFaultHandler::handle(arch::InterruptFrame *frame)
    {
        u64 errcode = frame->error_code;
        u64 fault_address = Register::read(CR2);
        panic("page fault in address %p with errcode %llx", fault_address, errcode);
    }
}