#include <io/io.hpp>

namespace io
{
void out8(const uint16_t port, const uint8_t value)
{
    asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

void out16(const uint16_t port, const uint16_t value)
{
    asm volatile("outw %%ax, %%dx" : : "d"(port), "a"(value));
}

void out32(const uint16_t port, const uint32_t value)
{
    asm volatile("outl %0, %1" : : "a"(value) , "Nd"(port));
}

uint8_t in8(const uint16_t port)
{
    uint8_t val;
    asm volatile("inb %1, %0"
                    : "=a"(val)
                    : "Nd"(port));
    return val;
}

uint16_t in16(const uint16_t port)
{
    uint16_t val;
    asm volatile("inw %1, %0"
                    : "=a"(val)
                    : "Nd"(port));
    return val;
}

uint32_t in32(const uint16_t port)
{
    uint32_t val;
    asm volatile("inl %1, %0"
                    : "=a"(val)
                    : "Nd"(port));
    return val;
}
}