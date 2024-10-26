#include <arch/cpu.hpp>
#include <arch/ipi.hpp>
#include <arch/ksyms.hpp>
#include <arch/interrupt_handler.hpp>
#include <dev/uart.hpp>
#include <lib/printf.hpp>

namespace kernel
{
    using namespace arch;

    void AbortIPIHandler::handle(InterruptFrame *)
    {
        CPU::disable_interrupts();
        printf("IPI ABORT FROM %u\n", CPU::current()->m_apicid);
        
        CPU::hnr();
    }
    
    void panic_write_out(char c, void *)
    {
        dev::UART::write(c);
    }

    void __no_sanitize__ __no_return__ __never_inline__ __panic(const char *file, const char *function, const char *fmt, ...)
    {
        // Disable interrupts so we do not jump around the kernel
        CPU::disable_interrupts();

        // Send an abort to the other cores
        IPI::send_interrupt(0, IPI_BROADCAST, IPIAbort);

        // Write the header
        char buf[1024];
        sprintf(buf, "\033[31m%s:%s: ", file, function);
        fctprintf(panic_write_out, nullptr, "%s", buf);
        
        // Write the message
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        fctprintf(panic_write_out, nullptr, "%s\n", buf);
        va_end(args);
        
        // Dump stack trace
        SymbolTable::backtrace();

        CPU::hnr();   
    }
}