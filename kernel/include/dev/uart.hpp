#pragma once

#include <kernel.hpp>

namespace dev
{
    class UART
    {
    private:
        static bool m_initialized;
    public:
        static void init();

        static void write(const char c);
        static void write(const char *s);
    };
}