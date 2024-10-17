#pragma once

#include <kernel.hpp>

namespace kernel::io
{
    void out8(const uint16_t port, const uint8_t value);
    void out16(const uint16_t port, const uint16_t value);
    void out32(const uint16_t port, const uint32_t value);

    uint8_t in8(const uint16_t port);
    uint16_t in16(const uint16_t port);
    uint32_t in32(const uint16_t port);
}