#pragma once

#include <types.hpp>
#include <attributes.h>
#include <limine.h>

constexpr u64 KiB = 1024;
constexpr u64 MiB = 1024 * KiB;
constexpr u64 GiB = 1024 * MiB;

#define __cli()     { asm volatile("cli" ::: "memory"); }
#define __sti()     { asm volatile("sti" ::: "memory"); }
#define __hlt()     { asm volatile("hlt" ::: "memory"); }
#define __pause()   { asm volatile("pause" ::: "memory"); }
#define __hnr() {   \
    __cli();        \
    while (1)       \
        __hlt();    \
}

extern volatile limine_bootloader_info_request bootloader_info_request;
extern volatile limine_hhdm_request hhdm_request;
extern volatile limine_memmap_request memmap_request;
extern volatile limine_kernel_address_request kernel_address_request;
extern volatile limine_kernel_file_request kernel_file_request;
extern volatile limine_framebuffer_request framebuffer_request;
extern volatile limine_smp_request smp_request;