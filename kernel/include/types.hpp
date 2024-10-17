#pragma once

#include <limits.h>
#include <stdint.h>
#include <stddef.h>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;
using ssize_t = long int;
using f32 = float;
using f64 = double;

constexpr u64 round_up(const u64 num, const size_t size)
{
    return (num + size - 1) & ~(size - 1);
}

constexpr u64 round_down(const u64 num, const size_t size)
{
    return (num & ~(size - 1));
}

constexpr bool is_aligned(const u64 num, const size_t size)
{
    return (bool)((num & (size - 1)) == 0);
}