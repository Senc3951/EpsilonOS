#include <arch/cpu.hpp>
#include <arch/register.hpp>
#include <arch/cpuid.hpp>
#include <arch/msr.hpp>
#include <log.hpp>

namespace kernel
{
    void CPU::setup()
    {
        // Streaming SIMD Extensions
        CPUID cpuid(1);
        if (cpuid.edx() & CpuidFlags::EDXSSE)
            sse_enable();
        else    // SSE is required
            panic("sse not supported");
        
        // Page Global Enable
        if (cpuid.edx() & CpuidFlags::EDXPGE)
            // Set CR4.PGE
            Register::write(CR4, Register::read(CR4) | Feature::CR4PGE);
        
        // XSAVE
        if (cpuid.ecx() & CpuidFlags::ECXXSAVE)
        {
            // Set CR4.OSXSAVE
            Register::write(CR4, Register::read(CR4) | Feature::CR4OSXSAVE);

            // According to the Intel manual: "After reset, all bits (except bit 0) in XCR0 are cleared to zero; XCR0[0] is set to 1."
            // Sadly we can't trust this, for example VirtualBox starts with bits 0-4 set, so let's do it ourselves.
            Register::write(XCR0, 1);

            // Advanced Vector Extensions
            if (cpuid.ecx() & CpuidFlags::ECXAVX)
                // Set XCR0.X87, XCR0.SSE, XCR0.AVX
                Register::write(XCR0, Register::read(XCR0) | Feature::XCR0X87 | Feature::XCR0SSE | Feature::XCR0AVX);
        }
        
        // Supervisor Memory Access Protection
        cpuid = CPUID(7);
        if (cpuid.ebx() & CpuidFlags::EBXSMAP)
            // Set CR4.SMAP
            Register::write(CR4, Register::read(CR4) | Feature::CR4SMAP);
        
        // Supervisor Memory Execute Protection
        if (cpuid.ebx() & CpuidFlags::EBXSMEP)
            // Set CR4.SMEP
            Register::write(CR4, Register::read(CR4) | Feature::CR4SMEP);
        
        // User Mode Instruction Prevention
        if (cpuid.ecx() & CpuidFlags::ECXUMIP)
            // Set CR4.UMIP
            Register::write(CR4, Register::read(CR4) | Feature::CR4UMIP);
        
        // TODO: syscall
    }
    
    void CPU::sse_enable()
    {
        // Clear CR0.EM, Set CR0.MP
        u64 cr0 = Register::read(CR0);
        cr0 &= ~Feature::CR0EM;
        Register::write(CR0, cr0 | Feature::CR0MP);
        
        // Set CR4.OSFXSR, CR4.OSXMMEXCPT
        Register::write(CR4, Register::read(CR4) | Feature::CR4OSFXSR | Feature::CR4OSXMMEXCPT);
    }
}