#pragma once

#include <types.hpp>
#include <attributes.h>
#include <limine.h>

namespace kernel
{
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

    extern volatile limine_bootloader_info_request bootloader_info_request;
    extern volatile limine_hhdm_request hhdm_request;
    extern volatile limine_memmap_request memmap_request;
    extern volatile limine_kernel_address_request kernel_address_request;
    extern volatile limine_kernel_file_request kernel_file_request;
    extern volatile limine_framebuffer_request framebuffer_request;
    extern volatile limine_smp_request smp_request;

    template <typename T>
    constexpr __always_inline__ T tohh(T ptr)
    {
        return reinterpret_cast<T>(reinterpret_cast<uintptr_t>(ptr) + hhdm_request.response->offset);
    }
    
    template <typename T>
    constexpr __always_inline__ T fromhh(T ptr)
    {
        return reinterpret_cast<T>(reinterpret_cast<uintptr_t>(ptr) - hhdm_request.response->offset);
    }
}