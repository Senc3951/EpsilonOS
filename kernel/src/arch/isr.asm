bits 64

%macro ISR_EXCEPTION 1
    global INT%1
    INT%1:
        %if !(%1 == 8 || (%1 >= 10 && %1 <= 14) || %1 == 17 || %1 == 21 || %1 == 29 || %1 == 30)
            push qword 0    ; push dummy error code
        %endif
        
        push qword %1   ; interrupt number
        jmp isr_common
%endmacro

section .text

%include "arch/util.inc"

extern isr_interrupt_handler
isr_common:
    cld
    pushaq
    
    ; store current data segment
    xor rax, rax
    mov ax, ds
    push rax

    ; handle inside kernel segments
    call set_data_segments_to_kernel
    
    ; enter c handler
    mov rdi, rsp
    call isr_interrupt_handler
    mov rsp, rax
        
    ; restore saved segments
    pop rax
    call restore_data_segments

    popaq           ; restore registers
    add rsp, 16     ; remove interrupt number and error code
        
    iretq

; generate handlers for all interrupts
%assign i 0
%rep 256
    ISR_EXCEPTION i
%assign i i+1
%endrep

section .data
global __interrupt_handlers
__interrupt_handlers:
    %assign i 0
    %rep 256
        dq INT%+i
    %assign i i+1
    %endrep