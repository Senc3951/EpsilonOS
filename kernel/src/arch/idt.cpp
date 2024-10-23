#include <arch/idt.hpp>
#include <log.hpp>

namespace kernel::arch
{
    constexpr u16 IDT_PRESENT = 1 << 7;
    constexpr u16 IDT_DPL0 = 0 << 5;
    constexpr u16 IDT_DPL3 = 3 << 5;
    constexpr u16 IDT_INTERRUPT = 0b1110;
    constexpr u16 IDT_TRAP = 0b1111;
    
    constexpr u16 IDT_INTERRUPT_TYPE0 = IDT_PRESENT | IDT_DPL0 | IDT_INTERRUPT;
    constexpr u16 IDT_INTERRUPT_TYPE3 = IDT_PRESENT | IDT_DPL3 | IDT_INTERRUPT;
    constexpr u16 IDT_TRAP_TYPE0 = IDT_PRESENT | IDT_DPL0 | IDT_TRAP;
    constexpr u16 IDT_TRAP_TYPE3 = IDT_PRESENT | IDT_DPL3 | IDT_TRAP;
    
    struct IdtEntry
    {
        uint16_t offset_low;
        uint16_t segment;
        uint8_t ist : 3;
        uint8_t reserved0 : 5;
        uint8_t flags;
        uint16_t offset_mid;
        uint32_t offset_high;
        uint32_t reserved1;
    } __packed__;
    
    static IdtEntry entries[IDT_ENTRIES];
    extern "C" uintptr_t __interrupt_handlers[];
    
    static void set_entry(const u8 index, const u64 offset, const u16 segment, const u16 flags)
    {
        IdtEntry *entry = &entries[index];
        
        entry->offset_low = static_cast<u16>(offset);
        entry->offset_mid = static_cast<u16>(offset >> 16);
        entry->offset_high = static_cast<u32>(offset >> 32);
        
        entry->segment = segment;
        entry->flags = flags;
        entry->ist = 0;
        entry->reserved0 = entry->reserved1 = 0;
    }

    void idt_init(CPU& cpu)
    {   
        // Set entries
        for (size_t i = 0; i < IDT_ENTRIES; i++)
            set_entry(i, __interrupt_handlers[i], KERNEL_CS, IDT_TRAP_TYPE0);
        
        // Set idt descriptor
        TableDescriptor& idtdesc = const_cast<TableDescriptor&>(cpu.get_idt());
        idtdesc.size = sizeof(entries) - 1;
        idtdesc.base = reinterpret_cast<u64>(entries);

        dmesgln("IDT at %p", &idtdesc);
    }
}