#include <stdlib.h>

__attribute__((__noreturn__)) void abort(void)
{
#ifdef __THEOS_KERNEL
    klog(FATAL, "Kernel panic: abort()\r\n");
#else
    puts("abort()\r\n");
#endif

    while (true) { }
    __builtin_unreachable();
}