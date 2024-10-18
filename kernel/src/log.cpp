#include <log.hpp>
#include <lib/printf.hpp>

namespace kernel
{
    void _log_message(const char *file, const bool critical, const char *fmt, ...)
    {
        va_list arg;
        va_start(arg, fmt);
        
        if (critical)
            printf("\033[31m%s: ", file);
        else
            printf("\033[36m%s: ", file);

        vprintf(fmt, arg);
        printf("\033[39m\n");
        
        va_end(arg);
    }
}