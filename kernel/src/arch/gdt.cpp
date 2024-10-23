#include <arch/gdt.hpp>

namespace kernel::arch
{
    constexpr u8 GDT_PRESENT = 1 << 7;
    constexpr u8 GDT_DPL0 = 0 << 5;
    constexpr u8 GDT_DPL3 = 3 << 5;
    constexpr u8 GDT_TYPE_SEGMENT = 1 << 4;
    constexpr u8 GDT_EXEC_DATA_SEGMENT = 0 << 3;
    constexpr u8 GDT_EXEC_CODE_SEGMENT = 1 << 3;
    constexpr u8 GDT_READ_CODE_SEGMENT = 1 << 1;
    constexpr u8 GDT_READ_DATA_SEGMENT = 1 << 1;
    
    constexpr u8 KERNEL_CODE_SEGMENT = GDT_PRESENT | GDT_DPL0 | GDT_TYPE_SEGMENT | GDT_EXEC_CODE_SEGMENT | GDT_READ_CODE_SEGMENT;
    constexpr u8 KERNEL_DATA_SEGMENT = GDT_PRESENT | GDT_DPL0 | GDT_TYPE_SEGMENT | GDT_EXEC_DATA_SEGMENT | GDT_READ_DATA_SEGMENT;
    constexpr u8 USER_CODE_SEGMENT = GDT_PRESENT | GDT_DPL3 | GDT_TYPE_SEGMENT | GDT_EXEC_CODE_SEGMENT | GDT_READ_CODE_SEGMENT;
    constexpr u8 USER_DATA_SEGMENT = GDT_PRESENT | GDT_DPL3 | GDT_TYPE_SEGMENT | GDT_EXEC_DATA_SEGMENT | GDT_READ_DATA_SEGMENT;

    struct GdtEntry
    {
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t base_middle;
        uint8_t access;
        uint8_t granularity;
        uint8_t base_high;
    } __packed__;

    static GdtEntry entries[6];

    static void set_entry(const u8 index, const u32 base, const u32 limit, const u8 access, const u8 granularity)
    {
        GdtEntry *entry = &entries[index];

        entry->limit_low = static_cast<u16>(limit);
        entry->base_low = static_cast<u16>(base);
        entry->base_middle = static_cast<u8>(base >> 16);
        entry->access = access;
        entry->granularity = (((limit >> 16) & 0x0F) | (granularity & 0xF0));
        entry->base_high = static_cast<u8>(base >> 24);
    }
    
    void gdt_init(CPU& cpu)
    {
        // Populate segments
        set_entry(0, 0, 0, 0, 0);                               // Kernel Null segment
        set_entry(1, 0, 0xFFFFF, KERNEL_CODE_SEGMENT, 0xAF);    // Kernel Code segment
        set_entry(2, 0, 0xFFFFF, KERNEL_DATA_SEGMENT, 0xAF);    // Kernel Data segment
        set_entry(3, 0, 0xFFFFF, USER_CODE_SEGMENT, 0xAF);      // User Code segment
        set_entry(4, 0, 0xFFFFF, USER_DATA_SEGMENT, 0xAF);      // User Cata segment    
        
        // Set gdt descriptor
        TableDescriptor& gdtdesc = const_cast<TableDescriptor&>(cpu.get_gdt());
        gdtdesc.size = sizeof(entries) - 1;
        gdtdesc.base = reinterpret_cast<u64>(entries);
    }
}