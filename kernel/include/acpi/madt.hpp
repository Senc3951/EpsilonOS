#pragma once

#include <kernel.hpp>

namespace kernel::acpi
{
    constexpr u16 MAX_IOAPIC = 16;

    struct IOAPIC
    {
        u8 id;
        u32 gsib;
        uintptr_t address;
    };

    class MADT
    {
    private:
        static MADT m_instance;
        
        uintptr_t m_lapic;
        u32 m_ioapic_count = 0;
        IOAPIC m_ioapic[MAX_IOAPIC];
        
        MADT() { }
    public:
        static constexpr MADT& instance() { return m_instance; }

        void init();

        constexpr uintptr_t lapic() const { return m_lapic; }
        constexpr u32 ioapic_count() const { return m_ioapic_count; }
        constexpr IOAPIC& ioapic(const u32 i) const { return const_cast<IOAPIC&>(m_ioapic[i]); }
        
        // Delete the copy constructor & assignment operator
        MADT(const MADT&) = delete;
        MADT& operator=(const MADT&) = delete;
    };
}