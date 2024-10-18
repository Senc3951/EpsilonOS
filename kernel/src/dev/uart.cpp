#include <dev/uart.hpp>
#include <io/io.hpp>

namespace kernel::dev
{
    using namespace io;

    constexpr u64 PORT = 0x3F8;
    bool UART::m_initialized = false;

    void UART::init()
    {
    #ifdef SERIAL_OUT
        out8(PORT + 1, 0x00);    // Disable all interrupts
        out8(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
        out8(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
        out8(PORT + 1, 0x00);    //                  (hi byte)
        out8(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
        out8(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
        out8(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
        out8(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
        out8(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

        // Check if serial is faulty (i.e: not same byte as sent)
        if (in8(PORT + 0) != 0xAE)
            return;

        // If serial is not faulty set it in normal operation mode
        // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
        out8(PORT + 4, 0x0F);
        m_initialized = true;
    #endif
    }

    void UART::write(const char c)
    {
        if (!m_initialized)
            return;

        while ((in8(PORT + 5) & 0x20) == 0) ;
        out8(PORT, c);
    }

    void UART::write(const char *s)
    {
        if (!m_initialized)
            return;
        
        const char *tmp = s;
        while (*tmp)
            write(*tmp++);
    }
}