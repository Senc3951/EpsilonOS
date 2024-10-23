#include <acpi/madt.hpp>
#include <acpi/rsdt.hpp>
#include <mem/address.hpp>

namespace kernel::acpi
{
    #define RSDT_APIC_NAME "APIC"

    constexpr u8 IO_APIC = 1;
    constexpr u8 LAPIC_OVERRIDE = 5;
    
    struct Record
    {
        u8 type;
        u8 length;
    } __packed__;

    struct IOAPICInternal
    {
        uint8_t id;
        uint8_t reserved;
        uint32_t address;
        uint32_t gsib;
    } __packed__;

    struct LAPICInternal
    {
        u16 reserved;
        u64 address;
    } __packed__;

    struct MADTInternal
    {
        RsdtHeader header;
        uint32_t lapic_address;
        uint32_t flags;
    } __packed__;

    MADT MADT::m_instance;

    void MADT::init()
    {
        MADTInternal *madt = reinterpret_cast<MADTInternal *>(RSDT::instance().find_table(RSDT_APIC_NAME));
        assert(madt && "failed finding apic (madt)");

        // Get the lapic address and convert it to higher half
        m_lapic = Address::tohh<u32, uintptr_t>(madt->lapic_address);
        
        LAPICInternal *internal_lapic;
        IOAPICInternal *internal_ioapic;
        IOAPIC *ioapic;

        // Iterate the entries
        Record *record;
        for (u8 *ptr = reinterpret_cast<u8 *>(madt) + sizeof(MADTInternal);
            ptr < reinterpret_cast<u8 *>(madt) + madt->header.length; ptr += record->length)
        {
            record = reinterpret_cast<Record *>(ptr);
            switch (record->type)
            {
                case IO_APIC:
                    internal_ioapic = reinterpret_cast<IOAPICInternal *>(record + 2);
                    ioapic = &m_ioapic[m_ioapic_count++];

                    ioapic->id = internal_ioapic->id;
                    ioapic->gsib = internal_ioapic->gsib;
                    ioapic->address = Address::tohh<u32, uintptr_t>(internal_ioapic->address);
                    
                    break;
                case LAPIC_OVERRIDE:
                    internal_lapic = reinterpret_cast<LAPICInternal *>(record + 2);
                    m_lapic = Address::tohh<u64, uintptr_t>(internal_lapic->address);
                    
                    break;
            }
        }
    }
}