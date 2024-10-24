#include <arch/ipi.hpp>
#include <arch/lapic.hpp>

namespace kernel::arch
{
    constexpr u8 DESTMOD_PHYSICAL = 0;
    constexpr u8 DESTMOD_SELF = 1;
    constexpr u8 DESTMOD_ALL_WITH_SELF = 2;
    constexpr u8 DESTMOD_ALL_BUT_SELF = 3;

    union ICR
    {
        struct
        {
            uint32_t vector:8, delivery_mode:3, dest_mode:1,
                delivery_status:1, reserved0:1, level:1,
                trigger:1, reserved1:2, dest_shorthand:2,
                reserved2:12;
            uint32_t reserved3:24, dest:8;
        } __packed__;
        
        struct
        {
            uint32_t value_low;
            uint32_t value_high;
        } __packed__;

        u64 value;
    };

    void IPI::send_interrupt(const u8 apicid, const u8 dest, const u8 vector)
    {
        send(apicid, IPIDelivery::Normal, vector, dest, 1, 0);
    }
    
    void IPI::send(const u8 apicid, const u8 delvmod, const u8 vector, const u8 dest, const u8 level, const u8 trigger)
    {
        if (!APIC::is_enabled())
            return;
        
        ICR icr = { 0 };
        
        icr.vector = vector;
        icr.delivery_mode = delvmod;

        switch (dest) {
            case IPI_BROADCAST:
                icr.dest_shorthand = DESTMOD_ALL_WITH_SELF;
                break;
            case IPI_SINGLE:
                icr.dest_mode = DESTMOD_PHYSICAL;
                icr.dest = apicid;
                break;
            default:
                panic("Unknown destination type %u", dest);
        }
        
        // "Level" and "deassert" are for 82489DX
        icr.level = level;
        icr.trigger = trigger;
        
        /* Writing the low doubleword of the ICR causes
        * the IPI to be sent: prepare high-word first. */
        APIC::write(LAPIC_ICRHI, icr.value_high);
        APIC::write(LAPIC_ICRLO, icr.value_low);   
    }
}