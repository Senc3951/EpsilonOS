#include <arch/ksyms.hpp>
#include <lib/string.h>
#include <log.hpp>

namespace kernel::arch
{
    extern "C" {
        __attribute__((section(".ksyms"))) char kernel_symbols[5 * MiB] {};
    }
    
    KernelSymbol *SymbolTable::m_symbols = nullptr;

    void SymbolTable::load()
    {
        char *bufptr = kernel_symbols;

        // First line is the number of lines (symbol count) in the exported map file
        u64 symbol_count = 0;
        for (size_t i = 0; i < sizeof(uintptr_t); i++)
            symbol_count = (symbol_count << 4) | hex2num(*(bufptr++));
        bufptr++;   // Skip the new line character
        
        // Allocate memory for the symbols
        m_symbols = new KernelSymbol[symbol_count];
        if (!m_symbols)
            panic("failed allocating memory for kernel symbols");
        
        // Calculate the actual size in bytes of the exported symbols
        size_t size = strlen(kernel_symbols);
        size_t current_symbol = 0, symbol_address = 0;

        // Load the symbols
        char *symbol_name;
        while (bufptr < kernel_symbols + size)
        {
            // Read the address
            for (size_t i = 0; i < sizeof(uintptr_t) * 2; i++)
                symbol_address = (symbol_address << 4) | hex2num(*(bufptr++));
            
            // Skip the spaces
            bufptr += 2;
            
            // Find the end-of-line
            symbol_name = bufptr;
            for (; *bufptr && *bufptr != '\n'; bufptr++) ;
            
            // Update the sybol
            KernelSymbol& symbol = m_symbols[current_symbol++];
            symbol.address = symbol_address;
            symbol.name = symbol_name;
            
            // Replace the new line with a null terminator and move to the next line
            *(bufptr++) = '\0';
        }
        
        dmesgln("Kernel symbols loaded");
    }

    constexpr u8 SymbolTable::hex2num(const u8 c)
    {
        if (c >= '0' && c <= '9')
            return c - '0';
        
        assert(c >= 'a' && c <= 'f');
        return 10 + (c - 'a');
    }
}