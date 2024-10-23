#pragma once

#include <kernel.hpp>

namespace kernel::arch
{
    enum Interrupt
    {
        Division = 0,
        Debug,
        NMI,
        Breakpoint,
        Overflow,
        BoundRange,
        InvalidOpcode,
        DeviceNotAvailable,
        DoubleFault,
        CoprocessorSegmentOverrun,
        InvalidTSS,
        SegmentNotPresent,
        StackSegment,
        GeneralProtection,
        PageFault,
        X87FloatPoint = 16,
        AlignmentCheck,
        MachineCheck,
        SIMDFloatingPoint,
        Virtualization,
        ControlProtection,
        HypervisorInjection = 28,
        VMMCommunication,
        Security,
        ApicSpurious = 0xFF
    };
    
    struct InterruptFrame
    {
        u64 ds;
        u64 r15, r14, r13, r12, r11, r10, r9, r8;
        u64 rbp, rsi, rdi, rdx, rcx, rbx, rax;
        u64 num, error_code;
        u64 rip, cs, rflags, rsp, ss;
    } __packed__;
}