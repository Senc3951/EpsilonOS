#pragma once

#include <cstdint>
#include <cstddef>
#include <climits>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

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