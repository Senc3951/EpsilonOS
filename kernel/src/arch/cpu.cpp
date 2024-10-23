#include <arch/cpu.hpp>
#include <arch/register.hpp>
#include <arch/cpuid.hpp>

namespace kernel
{
    extern "C" void x64_load_gdt(const uintptr_t gdt, const u64 cs, const u64 ds);
    extern "C" void x64_load_idt(const uintptr_t idt);

    static CPU cpus[MAX_CPU];
    u32 CPU::m_id = 0;

    CPU& CPU::init()
    {
        // Streaming SIMD Extensions
        CPUID cpuid(1);
        assert((cpuid.edx() & CpuidFlags::EDXSSE) && "sse not supported");
        
        // Enable SSE
        sse_enable();
        
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
        
        // Page Attribute Table
        if (cpuid.edx() & CpuidFlags::EDXPAT)
        {
            MSR msr(MSR_IA32_PAT);

            /* Set PAT entry 4 to WC. This allows us to
            use this mode by only setting the bit in the PTE
            and leaving all other bits in the upper levels unset,
            which maps to setting bit 3 of the index, resulting in the index value 0 or 4. */

            u64 pat = msr.read() & ~(0x7ULL << 32);  // Clear entry 4
            pat |= (0x1ULL << 32);                  // Set WC in entry 4
            msr.write(pat);
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
        
        // FS / GS
        assert((cpuid.ebx() & CpuidFlags::EBXFSGS) && "%%fs & %%gs are inaccessible");
        // Set CR4.FSGSBASE
        Register::write(CR4, Register::read(CR4) | Feature::CR4FSGS);
        
        // TODO: syscall

        // Return a new instance to a CPU
        return get_next_cpu();
    }
    
    void CPU::flush_tlb(const u64 virt) const
    {
        asm volatile("invlpg (%0)" ::"r" (virt) : "memory");
    }

    void CPU::write_self_to_gs() const
    {
        MSR msr(MSR_IA32_GS);
        msr.write(reinterpret_cast<uintptr_t>(this));
    }

    void CPU::load_gdt()
    {
        x64_load_gdt(reinterpret_cast<uintptr_t>(&m_gdt), KERNEL_CS, KERNEL_DS);
    }
    
    void CPU::load_idt()
    {
        x64_load_idt(reinterpret_cast<uintptr_t>(&m_idt));
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

    CPU& CPU::get_next_cpu()
    {
        assert(m_id < MAX_CPU);
        return cpus[m_id++];
    }
}