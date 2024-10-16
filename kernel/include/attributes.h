#pragma once

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