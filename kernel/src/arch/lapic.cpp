#include <arch/lapic.hpp>
#include <arch/interrupt.hpp>
#include <arch/cpuid.hpp>
#include <arch/isr.hpp>
#include <acpi/madt.hpp>
#include <log.hpp>

namespace kernel::arch
{
    class ApicSpuriousInterruptHandler final : public IRQHandler
    {
    public:
        virtual void handle(InterruptFrame *frame) override
        {
            critical_dmesgln("Spurious interrupt at %llxx%p", frame->cs, frame->rip);
        }
    };
    
    uintptr_t APIC::m_lapic;
    bool APIC::m_enabled = false;
    
    void APIC::init(CPU& cpu)
    {
        // Get the apic address (only runs once)
        if (!m_enabled)
            m_lapic = acpi::MADT::lapic();
        
        // Check that apic is supported
        assert(is_supported() && "apic not supported");
        
        // Enale apic
        enable();
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
        
        // Set current apicid at the local cpu struct
        cpu.m_apicid = id();

        // Register the spurious interrupt handler (only run once)
        if (!m_enabled)
        {
            m_enabled = true;        
            InterruptManager::register_interrupt<ApicSpuriousInterruptHandler>(ApicSpurious);
            
            critical_dmesgln("LAPICv%u at %p. Running with %u core(s)", static_cast<u8>(read(LAPIC_VER)), m_lapic, smp_request.response->cpu_count);
        }
    }
    
    bool APIC::is_supported()
    {
        CPUID cpuid(1);
        return cpuid.edx() & EDXAPIC;
    }

    void APIC::enable()
    {
        constexpr u32 IA32_APIC_BASE_MSR_ENABLE = 1 << 11;
        
        u32 edx = 0;
        u32 eax = (m_lapic & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE;

        #ifdef __PHYSICAL_MEMORY_EXTENSION__
            edx = (m_lapic >> 32) & 0x0f;
        #endif
        
        MSR::write(MSR_IA32_APIC, eax, edx);
    }
}