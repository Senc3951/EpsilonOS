bits 64

section .text
global x64_load_gdt
x64_load_gdt:
    lgdt [rdi]  ; gdt

    ; reload code segment
    push rsi    ; cs
    lea rax, [rel .after_reload]
    push rax
    retfq
.after_reload:
    ; reload data segment
    mov ax, dx  ; ds
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ret

global x64_load_idt
x64_load_idt:
    lidt [rdi]  ; idt
    ret