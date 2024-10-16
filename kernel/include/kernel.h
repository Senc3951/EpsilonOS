#pragma once

#include <cstdint>
#include <cstddef>
#include <climits>
#include <limine.h>

#ifdef __packed__
#    undef __packed__
#else
#    define __packed__ __attribute__((packed))
#endif

#ifdef __no_return__
#    undef __no_return__
#else
#    define __no_return__ __attribute__((noreturn))
#endif

#ifdef __malloc__
#    undef __malloc__
#else
#    define __malloc__ __attribute__((__malloc__))
#endif

#ifdef __always_inline__
#    undef __always_inline__
#else
#    define __always_inline__ __attribute__((always_inline)) inline
#endif

#ifdef __never_inline__
#    undef __never_inline__
#else
#    define __never_inline__ __attribute__((noinline))
#endif

#ifdef __limine_request__
#   undef __limine_request__
#else
#   define __limine_request__ __attribute__((used, section(".requests")))
#endif

#define rndup(n, s)    (((n) + (s) - 1) & ~((s) - 1))
#define rndown(n, s)   ((n) & ~((s) - 1))

extern volatile limine_bootloader_info_request bootloader_info_request;
extern volatile limine_hhdm_request hhdm_request;
extern volatile limine_memmap_request memmap_request;
extern volatile limine_kernel_address_request kernel_address_request;
extern volatile limine_kernel_file_request kernel_file_request;
extern volatile limine_framebuffer_request framebuffer_request;
extern volatile limine_smp_request smp_request;