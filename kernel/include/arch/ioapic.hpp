#pragma once

#include <acpi/madt.hpp>

namespace kernel::arch
{
    class IOAPIC
    {
    private:
        static constexpr u64 IOREGSEL = 0;
        static constexpr u64 IOWIN = 0x10;
        static constexpr u64 IOREDTBL0 = 0x10;
        
        static constexpr u32 REGID = 0;
        static constexpr u32 REGVER = 1;

        static inline uint32_t read(const uintptr_t base, const uintptr_t offset)
        {
            *(volatile uintptr_t *)((base) + IOREGSEL) = offset;
            return *(volatile uintptr_t *)((base) + IOWIN);
        }
        
        static constexpr void write(const uintptr_t base, const uintptr_t offset, const uint32_t value)
        {
            *(volatile uintptr_t *)((base) + IOREGSEL) = offset;
            *(volatile uintptr_t *)((base) + IOWIN) = value;
        }
        
        static constexpr void map_entry(const uintptr_t base, const u8 index, const u64 val)
        {
            write(base, IOREDTBL0 + index * 2, static_cast<u32>(val));
            write(base, IOREDTBL0 + index * 2 + 1, static_cast<u32>(val >> 32));
        }

        static inline u16 get_max_interrupts(acpi::IOAPIC& ioapic)
        {
            return static_cast<u8>(read(ioapic.address, REGVER) >> 16) + 1;
        }
        
        static inline u32 get_id(acpi::IOAPIC& ioapic)
        {
            return (read(ioapic.address, REGID) >> 24) & 0xF;
        }
        
        static inline u16 get_version(acpi::IOAPIC& ioapic)
        {
            return static_cast<u8>(read(ioapic.address, REGVER));
        }
    
        static acpi::IOAPIC *find_ioapic(const u8 irq);
    public:
        static void init();
        static void map(const u8 irq, const u64 apicid);
    };
}