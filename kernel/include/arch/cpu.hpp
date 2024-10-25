#pragma once

#include <arch/msr.hpp>

namespace kernel
{
    constexpr u32 MAX_CPU = 64;
    constexpr u64 KERNEL_CS = 0x8;
    constexpr u64 KERNEL_DS = 0x10;
    
    struct TableDescriptor
    {
        u16 size;
        u64 base;
    } __packed__;

    class CPU
    {
    private:
        __aligned__(sizeof(u32)) TableDescriptor m_gdt;
        __aligned__(sizeof(u32)) TableDescriptor m_idt;
        static u32 m_id;

        static void sse_enable();
        static CPU& get_next_cpu();
    public:
        u32 m_apicid;

        void flush_tlb(const uintptr_t virt) const;
        void write_self_to_gs() const;

        void load_gdt();
        constexpr const TableDescriptor& get_gdt() const { return m_gdt; }
        void load_idt();
        constexpr const TableDescriptor& get_idt() const { return m_idt; }
        
        static CPU& init(); /* Returns a new instance of a CPU */
        static __no_sanitize__ __always_inline__ __pure_const__ CPU *current()
        {
            return reinterpret_cast<CPU *>(MSR::read(MSR_IA32_GS));
        }
        
        static __always_inline__ void enable_interrupts() { asm volatile("sti" ::: "memory"); }
        static __always_inline__ void disable_interrupts() { asm volatile("cli" ::: "memory"); }
        static __always_inline__ void halt() { asm volatile("hlt" ::: "memory"); }
        static __always_inline__ __no_return__ void hnr() {
            disable_interrupts();
            while (true)
                halt();
        }        
    };
}