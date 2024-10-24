#include <arch/ioapic.hpp>
#include <arch/lapic.hpp>
#include <log.hpp>

namespace kernel::arch
{
    using namespace acpi;

    constexpr u32 IOAPIC_MASKED = 1 << 16;
    
    void IOAPIC::init()
    {
        for (u32 i = 0; i < MADT::instance().ioapic_count(); i++)
        {
            acpi::IOAPIC& ioapic = MADT::instance().ioapic(i);
            
            // Verify ID
            uint8_t id = get_id(ioapic);
            assert(id == ioapic.id);
            
            // Disable all interrupts and map as fixed, physical, active high, edge triggered
            u16 max_interrupts = get_max_interrupts(ioapic);
            for (u16 i = 0; i < max_interrupts; i++){}
                map_entry(ioapic.address, i, (IRQ0 + i) | IOAPIC_MASKED);
            
            dmesgln("(%u) I/O APICv%u at %p can map irq %u-%u", i, get_version(ioapic), ioapic.address, ioapic.gsib, ioapic.gsib + max_interrupts);
        }
    }
    
    void IOAPIC::map(const u8 irq, const u64 apicid)
    {
        // Get ioapic by interrupt
        acpi::IOAPIC *ioapic = find_ioapic(irq);
        if (!ioapic)
            panic("failed finding i/o apic available to map irq %u\n", irq);
        
        // Map as fixed, physical, active high, edge triggered
        uint64_t value = (IRQ0 + irq) | (apicid << 56);
        map_entry(ioapic->address, irq, value);
        
        critical_dmesgln("Mapped irq %x to apicid %x using ioapic at %p", irq, apicid, ioapic->address);
    }

    acpi::IOAPIC *IOAPIC::find_ioapic(const u8 irq)
    {
        for (u32 i = 0; i < MADT::instance().ioapic_count(); i++)
        {
            acpi::IOAPIC& ioapic = MADT::instance().ioapic(i);
            if (irq >= ioapic.gsib && irq <= ioapic.gsib + get_max_interrupts(ioapic))
                return &ioapic;
        }

        return nullptr;
    }
}