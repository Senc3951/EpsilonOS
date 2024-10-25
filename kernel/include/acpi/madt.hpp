#pragma once

#include <kernel.hpp>

namespace kernel::acpi
{
    struct IOAPIC
    {        
        u8 id;
        u32 gsib;
        uintptr_t address;
    };

    class MADT
    {
    private:
        static constexpr u16 MAX_IOAPIC = 16;
        
        static uintptr_t m_lapic;
        static u32 m_ioapic_count;
        static IOAPIC m_ioapic[MAX_IOAPIC];
    public:
        static void init();

        static inline uintptr_t lapic() { return m_lapic; }
        static inline u32 ioapic_count() { return m_ioapic_count; }
        static inline IOAPIC& ioapic(const u32 i) { return m_ioapic[i]; }
    };
}