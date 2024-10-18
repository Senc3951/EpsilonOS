#pragma once

#include <kernel.hpp>

namespace kernel
{
    enum Feature
    {
        CR0MP           = 1 << 1,
        CR0EM           = 1 << 2,
        CR4PGE          = 1 << 7,
        CR4OSFXSR       = 1 << 9,
        CR4OSXMMEXCPT   = 1 << 10,
        CR4UMIP         = 1 << 11,
        CR4FSGS         = 1 << 16,
        CR4OSXSAVE      = 1 << 18,
        CR4SMEP         = 1 << 20,
        CR4SMAP         = 1 << 21,
        XCR0X87         = 1 << 0,
        XCR0SSE         = 1 << 1,
        XCR0AVX         = 1 << 2
    };
    
    enum RegisterName
    {
        CR0,
        CR2,
        CR3,
        CR4,
        XCR0
    };

    class Register
    {
    public:
        static u64 read(const RegisterName& name);
        static void write(const RegisterName &name, const u64 value);
    };
}