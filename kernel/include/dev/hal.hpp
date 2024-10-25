#pragma once

#include <kernel.hpp>

namespace kernel::dev
{
    class HAL
    {
    public:
        static void init();

        static void msleep(const u64 ms);
        static void oneshot(const u64 ms);
        static void periodic(const u64 ms);
    };
}