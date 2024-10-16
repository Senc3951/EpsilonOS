#pragma once

#include <kernel.hpp>

#ifdef SERIAL_OUT
#   define dmesgln(...)            _log_message(__FILE__, false, __VA_ARGS__)
#   define critical_dmesgln(...)   _log_message(__FILE__, true, __VA_ARGS__)
#else
#   define dmesgln(...) { }
#   define critical_dmesgln(...) {}
#endif

void _log_message(const char *file, const bool critical, const char *fmt, ...);