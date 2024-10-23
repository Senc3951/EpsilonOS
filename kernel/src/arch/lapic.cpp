#include <arch/lapic.hpp>
#include <arch/interrupt.hpp>
#include <arch/cpuid.hpp>
#include <acpi/madt.hpp>
#include <log.hpp>

namespace kernel::arch
{
    uintptr_t APIC::m_lapic;
    
    void APIC::init(CPU& cpu)
    {
        // Check that apic is supported
        m_lapic = acpi::MADT::instance().lapic();
        assert(is_supported() && "apic not supported");

        // Hardware enale apic
        software_enable();
        
        // Software enable apic
        write(LAPIC_SVR, ApicSpurious | (1 << 8)); // Lower byte is interrupt number, bit 8 to enable
        
        // Disable logical interrupt lines
        write(LAPIC_LINT0, LAPIC_MASKED);
        write(LAPIC_LINT1, LAPIC_MASKED);
        
        // Disable performance counter overflow interrupts on machine that provide that interrupt entry
        if (((read(LAPIC_VER) >> 16) & 0xFF) >= 4)
            write(LAPIC_PERF, LAPIC_MASKED);
        
        // Clear error status register
        write(LAPIC_ESR, 0);
        write(LAPIC_ESR, 0);

        // Acknowledge any interrupt
        eoi();
        
        // Send an Init level De-Assert to synchronize arbitration id's
        write(LAPIC_ICRHI, 0);
        write(LAPIC_ICRLO, LAPIC_BCAST | LAPIC_INIT | LAPIC_LEVEL);
        while (read(LAPIC_ICRLO) & LAPIC_DELIVS) ;
        
        // Enable interrupts on the APIC
        write(LAPIC_TPR, 0);
        critical_dmesgln("LAPICv%u at %p. Running with %u core(s)", static_cast<u8>(read(LAPIC_VER)), m_lapic, smp_request.response->cpu_count);
        
        // Set current apicid at the local cpu struct
        cpu.m_apicid = id();
    }
    
    bool APIC::is_supported()
    {
        CPUID cpuid(1);
        return cpuid.edx() & EDXAPIC;
    }

    void APIC::software_enable()
    {
        #define IA32_APIC_BASE_MSR_ENABLE   (1 << 11)
        #define IA32_APIC_BASE_MSR          0x1B
            
        u32 edx = 0;
        u32 eax = (m_lapic & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE;

        #ifdef __PHYSICAL_MEMORY_EXTENSION__
            edx = (m_lapic >> 32) & 0x0f;
        #endif
        
        MSR::write(MSR_IA32_APIC, ((u64)edx << 32) | eax);
    }
}