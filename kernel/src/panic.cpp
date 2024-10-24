#include <lib/printf.hpp>
#include <arch/cpu.hpp>
#include <arch/ipi.hpp>
#include <arch/interrupt.hpp>
#include <dev/uart.hpp>

namespace kernel
{
    static void write_out(char c, void *)
    {
        dev::UART::write(c);
    }

    void __no_sanitize__ __no_return__ __never_inline__ __panic(const char *file, const char *function, const char *fmt, ...)
    {
        // Disable interrupts so we do not jump around the kernel
        CPU::disable_interrupts();

        // Send an abort to the other cores
        arch::IPI::send_interrupt(0, arch::IPI_BROADCAST, arch::Interrupt::IPIAbort);
        
        // Write the header
        char buf[1024];
        sprintf(buf, "\033[31m%s:%s: ", file, function);
        fctprintf(write_out, NULL, "%s", buf);
        
        // Write the message
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        fctprintf(write_out, NULL, "%s\033[39m\n", buf);
        va_end(args);

        CPU::hnr();   
    }
}