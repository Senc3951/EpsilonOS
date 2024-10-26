/* Tell the linker that we want an x86_64 ELF64 output file */
OUTPUT_FORMAT(elf64-x86-64)

/* We want the symbol kmain to be our entry point */
ENTRY(kmain)

/* Define the program headers we want so the bootloader gives us the right */
/* MMU permissions; this also allows us to exert more control over the linking */
/* process. */
PHDRS
{
    requests PT_LOAD;
    text     PT_LOAD;
    data     PT_LOAD;
    rodata   PT_LOAD;
    ksyms    PT_LOAD;
}

SECTIONS
{
    /* We want to be placed in the topmost 2GiB of the address space, for optimisations */
    /* and because that is what the Limine spec mandates. */
    /* Any address in this region will do, but often 0xffffffff80000000 is chosen as */
    /* that is the beginning of the region. */
    . = 0xffffffff80000000;
    
    /* Define a section to contain the Limine requests and assign it to its own PHDR */
    _requests_start = .;
    .requests : {
        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    } :requests

    /* Move to the next memory page */
    . = ALIGN(CONSTANT(MAXPAGESIZE));

    _requests_end = .;
    _text_start = .;

    .text : {
        *(.text .text.*)
    } :text

    /* Move to the next memory page */
    . = ALIGN(CONSTANT(MAXPAGESIZE));
    
    _text_end = .;
    _rodata_start = .;

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    /* Move to the next memory page */
    . = ALIGN(CONSTANT(MAXPAGESIZE));

    _rodata_end = .;
    _ctor_start = .;

    /* C++ is a language that allows for global constructors. In order to obtain the */
    /* address of the ".init_array" section we need to define a symbol for it. */
    .init_array : {
        __init_array = .;
        *(.init_array .init_array.*)
        __init_array_end = .;
    } :rodata

    /* Move to the next memory page */
    . = ALIGN(CONSTANT(MAXPAGESIZE));

    _ctor_end = .;
    _data_start = .;

    .data : {
        *(.data .data.*)
    } :data

    /* NOTE: .bss needs to be the last thing mapped to :data, otherwise lots of */
    /* unnecessary zeros will be written to the binary. */
    /* If you need, for example, .init_array and .fini_array, those should be placed */
    /* above this. */
    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data
    
    /* Move to the next memory page */
    . = ALIGN(CONSTANT(MAXPAGESIZE));
    
    _data_end = .;
    _ksyms_start = .;

    .ksyms : {
        KEEP(*(.ksyms))
    } :ksyms
    
    /* Make the kernel end address page-aligned */
    . = ALIGN(CONSTANT(MAXPAGESIZE));
    _ksyms_end = .;

    /* Discard .note.* and .eh_frame* since they may cause issues on some hosts. */
    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }
}