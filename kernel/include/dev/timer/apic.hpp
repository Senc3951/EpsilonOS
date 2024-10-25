#pragma once

#include <kernel.hpp>

namespace kernel::dev::timer
{
    class APICTimer
    {
    private:
        static u64 m_ticks_per_ms;

        static void set_counter(const u64 ms);
    public:
        static void init();
        
        static void msleep(const u64 ms);
        static void oneshot(const u64 ms);
        static void periodic(const u64 ms);
    };
}