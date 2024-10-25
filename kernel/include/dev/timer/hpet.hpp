#pragma once

#include <acpi/rsdt.hpp>

namespace kernel::dev::timer
{
    class HPET
    {
    private:
        static HPET m_instance;
        
        struct HpetTable
        {
            acpi::RsdtHeader header;
            u8 hardware_rev_id;
            u8 comparator_count : 5;
            u8 counter_size : 1;
            u8 reserved : 1;
            u8 legacy_replacement : 1;
            u16 pci_vendor_id;
            u8 address_space_id;
            u8 register_bit_width;
            u8 register_bit_offset;
            u8 reserved2;
            u64 address;
            u8 hpet_number;
            u16 minimum_tick;
            u8 page_protection;
        } __packed__;

        struct HpetRegs
        {
            u64 capabilities;
            u64 rsvd0;
            u64 general_config;
            u64 rsvd1;
            u64 interrupt_status;
            u64 rsvd2;
            u64 rsvd3[24];
            volatile u64 counter_value;
            u64 unused;
        } __packed__;

        HpetTable *m_hpet;
        HpetRegs *m_regs;

        HPET() { }
    public:
        static constexpr HPET& instance() { return m_instance; }

        void init();
        void msleep(const u64 ms);

        // Delete the copy constructor & assignment operator
        HPET(const HPET&) = delete;
        HPET& operator=(const HPET&) = delete;
    };
}