#pragma once

#include <kernel.hpp>

namespace kernel::arch
{
    struct KernelSymbol
    {
        uintptr_t address;
        const char *name;
    };

    class SymbolTable
    {
    private:
        static KernelSymbol *m_symbols;

        static constexpr u8 hex2num(const u8 c);
    public:
        static void load();
    };
}