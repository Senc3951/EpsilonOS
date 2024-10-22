#pragma once

#ifdef __packed__
#    undef __packed__
#else
#    define __packed__ __attribute__((packed))
#endif

#ifdef __aligned__
#   undef __aligned__
#else
#   define __aligned__(n) __attribute__((aligned(n)))
#endif

#ifdef __no_sanitize__
#   undef __no_sanitize__
#else
#   define __no_sanitize__ __attribute__((no_sanitize_coverage))
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

#ifdef __pure_const__
#   undef __pure_const__
#else
#   define __pure_const__ __attribute__((const))
#endif

#ifdef __limine_request__
#   undef __limine_request__
#else
#   define __limine_request__ __attribute__((used, section(".requests")))
#endif