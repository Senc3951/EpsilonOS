#include <arch/ksyms.hpp>
#include <lib/string.h>
#include <lib/printf.hpp>
#include <log.hpp>

namespace kernel::arch
{
    extern "C" {
        __attribute__((section(".ksyms"))) char kernel_symbols[5 * MiB] {};
    }
    
    KernelSymbol *SymbolTable::m_symbols = nullptr;
    u64 SymbolTable::m_symbol_count = 0;
    uintptr_t SymbolTable::m_lowest_addr = UINTPTR_MAX, SymbolTable::m_highest_addr = 0;
    bool SymbolTable::m_initialized = false;

    void SymbolTable::load()
    {
        char *bufptr = kernel_symbols;

        // First line is the number of lines (symbol count) in the exported map file
        for (size_t i = 0; i < sizeof(uintptr_t); i++)
            m_symbol_count = (m_symbol_count << 4) | hex2num(*(bufptr++));
        bufptr++;   // Skip the new line character
        
        // Allocate memory for the symbols
        m_symbols = new KernelSymbol[m_symbol_count];
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

            if (symbol_address > m_highest_addr)
                m_highest_addr = symbol_address;
            if (symbol_address < m_lowest_addr)
                m_lowest_addr = symbol_address;
            
            // Replace the new line with a null terminator and move to the next line
            *(bufptr++) = '\0';
        }
        
        m_initialized = true;
        dmesgln("Loaded %llu kernel symbols", m_symbol_count);
    }

    void SymbolTable::backtrace()
    {
        if (!m_initialized)
            return;
        
        struct StackFrame
        {
            struct StackFrame *rbp;
            u64 rip;
        };
        
        // Load the current rbp
        StackFrame *stack;
        asm volatile("mov %%rbp,%0" : "=r"(stack) ::);
        
        for (size_t frame = 0; stack && stack->rip && frame < MAX_SYMBOL_TRACE; frame++)
        {
            // Try to get the symbol of the current address
            KernelSymbol *symbol = addr2symbol(stack->rip);
            if (symbol)
            {
                u64 offset = stack->rip - symbol->address;
                fctprintf(panic_write_out, nullptr, "    at %p: %s(+%lld)\n", stack->rip, symbol->name, offset);
            }
            else
                fctprintf(panic_write_out, nullptr, "    at %p\n", stack->rip);
            
            stack = stack->rbp;
        }
    }

    constexpr u8 SymbolTable::hex2num(const u8 c)
    {
        if (c >= '0' && c <= '9')
            return c - '0';
        
        assert(c >= 'a' && c <= 'f');
        return 10 + (c - 'a');
    }

    KernelSymbol *SymbolTable::addr2symbol(const uintptr_t addr)
    {
        if (addr < m_lowest_addr || addr > m_highest_addr)
            return nullptr;
        
        for (u64 i = 0; i < m_symbol_count; i++)
        {
            if (addr < m_symbols[i + 1].address)
                return &m_symbols[i];
        }
        
        return nullptr;            
    }
}