#pragma once

#include <kernel.hpp>

namespace kernel::arch
{
    static constexpr u8 IPI_SINGLE = 0;
    static constexpr u8 IPI_BROADCAST = 1;

    enum IPIDelivery
    {
        Normal = 0,
        Lowest,
        SMI,
        NMI = 4,
        INIT,
        SIPI
    };

    class IPI
    {
    private:
        static void send(const u8 apicid, const u8 delvmod, const u8 vector, const u8 dest, const u8 level, const u8 trigger);
    public:
        static void send_interrupt(const u8 apicid, const u8 dest, const u8 vector);
    };
}