#pragma once

#include <arch/cpu.hpp>

namespace kernel::arch
{
    constexpr u32 LAPIC_ID          = 0x0020;  // Local APIC ID
    constexpr u32 LAPIC_VER         = 0x0030;  // Local APIC Version
    constexpr u32 LAPIC_TPR         = 0x0080;  // Task Priority
    constexpr u32 LAPIC_APR         = 0x0090;  // Arbitration Priority
    constexpr u32 LAPIC_PPR         = 0x00a0;  // Processor Priority
    constexpr u32 LAPIC_EOI         = 0x00b0;  // EOI
    constexpr u32 LAPIC_RRD         = 0x00c0;  // Remote Read
    constexpr u32 LAPIC_LDR         = 0x00d0;  // Logical Destination
    constexpr u32 LAPIC_DFR         = 0x00e0;  // Destination Format
    constexpr u32 LAPIC_SVR         = 0x00f0;  // Spurious Interrupt Vector
    constexpr u32 LAPIC_ISR         = 0x0100;  // In-Service (8 registers)
    constexpr u32 LAPIC_TMR         = 0x0180;  // Trigger Mode (8 registers)
    constexpr u32 LAPIC_IRR         = 0x0200;  // Interrupt Request (8 registers)
    constexpr u32 LAPIC_ESR         = 0x0280;  // Error Status
    constexpr u32 LAPIC_ICRLO       = 0x0300;  // Interrupt Command
    constexpr u32 LAPIC_ICRHI       = 0x0310;  // Interrupt Command [63:32]
    constexpr u32 LAPIC_TIMER       = 0x0320;  // LVT Timer
    constexpr u32 LAPIC_THERMAL     = 0x0330;  // LVT Thermal Sensor
    constexpr u32 LAPIC_PERF        = 0x0340;  // LVT Performance Counter
    constexpr u32 LAPIC_LINT0       = 0x0350;  // LVT LINT0
    constexpr u32 LAPIC_LINT1       = 0x0360;  // LVT LINT1
    constexpr u32 LAPIC_ERROR       = 0x0370;  // LVT Error
    constexpr u32 LAPIC_TICR        = 0x0380;  // Initial Count (for Timer)
    constexpr u32 LAPIC_TCCR        = 0x0390;  // Current Count (for Timer)
    constexpr u32 LAPIC_TDCR        = 0x03e0;  // Divide Configuration (for Timer)
    
    constexpr u32 LAPIC_INIT        = 0b101 << 8;    // INIT/RESET
    constexpr u32 LAPIC_STARTUP     = 0b110 << 8;    // Startup IPI
    constexpr u32 LAPIC_DELIVS      = 1 << 12;       // Delivery status
    constexpr u32 LAPIC_ASSERT      = 1 << 14;       // Assert interrupt (vs deassert)
    constexpr u32 LAPIC_DEASSERT    = 0 << 14;
    constexpr u32 LAPIC_LEVEL       = 1 << 15;       // Level triggered
    constexpr u32 LAPIC_BCAST       = 0b10 << 18;    // Send to all, including self.
    constexpr u32 LAPIC_BCAST_EX    = 0b11 << 18;    // Send to all, not including self.
    constexpr u32 LAPIC_MASKED      = 1 << 16;
    
    class APIC
    {
    private:
        static uintptr_t m_lapic;
        static bool m_enabled;

        static bool is_supported();
        static void enable();
    public:
        static void init(CPU& cpu);

        static inline bool is_enabled() { return m_enabled; }
        
        static __always_inline__ void eoi() { write(LAPIC_EOI, 0); }
        static __always_inline__ u8 id() { return read(LAPIC_ID) >> 24; }
        
        static __always_inline__ u32 read(const u32 offset) { return *reinterpret_cast<volatile u32 *>(m_lapic + offset); }
        static __always_inline__ void write(const u32 offset, const u32 value) { *reinterpret_cast<volatile u32*>(m_lapic + offset) = value; }
    };
}