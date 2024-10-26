#pragma once

#include <types.hpp>
#include <attributes.h>
#include <limine.h>

namespace kernel
{
    #define __pause() { asm volatile("pause" ::: "memory"); }
    #define __barrier() { asm volatile("" ::: "memory"); }
    
    /* panic & assert use this function to print messages */
    void panic_write_out(char c, void *);

    void __no_sanitize__ __no_return__ __never_inline__ __panic(const char *file, const char *function, const char *fmt, ...);
    #define panic(...)                                      \
        do {                                                \
            __panic(__FILE__, __FUNCTION__, __VA_ARGS__);   \
        } while (0)

    #define assert(expr)                                    \
        do {                                                \
            if (!(expr))                                    \
                panic("Assertation `%s` failed", #expr);    \
        } while (0)

    constexpr u64 KiB = 1024;
    constexpr u64 MiB = 1024 * KiB;
    constexpr u64 GiB = 1024 * MiB;

    constexpr size_t PAGE_SIZE = 4096;
    
    constexpr u32 IRQ0 = 0x20;
    
    // Limine requests
    extern volatile limine_bootloader_info_request bootloader_info_request;
    extern volatile limine_hhdm_request hhdm_request;
    extern volatile limine_rsdp_request rsdp_request;
    extern volatile limine_module_request module_request;
    extern volatile limine_memmap_request memmap_request;
    extern volatile limine_kernel_address_request kernel_address_request;
    extern volatile limine_kernel_file_request kernel_file_request;
    extern volatile limine_framebuffer_request framebuffer_request;
    extern volatile limine_smp_request smp_request;

    // Linker exports
    extern "C" void *_requests_start;
    extern "C" void *_text_start;
    extern "C" void *_rodata_start;
    extern "C" void *_ctor_start;
    extern "C" void *_ksyms_start;
    extern "C" void *_data_start;
    extern "C" void *_requests_end;
    extern "C" void *_text_end;
    extern "C" void *_rodata_end;
    extern "C" void *_ctor_end;
    extern "C" void *_ksyms_end;
    extern "C" void *_data_end;
}

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
void operator delete(void *p, size_t);
void operator delete[](void *p);
void operator delete[](void *p, size_t);