#pragma once

#include <kernel.hpp>

namespace kernel::arch
{
    constexpr size_t MAX_SYMBOL_TRACE = 64;
    
    struct KernelSymbol
    {
        uintptr_t address;
        const char *name;
    };

    class SymbolTable
    {
    private:
        static KernelSymbol *m_symbols;
        static u64 m_symbol_count;
        static uintptr_t m_lowest_addr, m_highest_addr;
        static bool m_initialized;
        
        static constexpr u8 hex2num(const u8 c);
        static KernelSymbol *addr2symbol(const uintptr_t addr);
    public:
        static void load();
        static void backtrace();
    };
}