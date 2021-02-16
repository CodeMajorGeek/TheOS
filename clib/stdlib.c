#include <stdlib.h>

#ifdef __THEOS_KERNEL
#include <kernel/logger.h>
#endif

__attribute__((__noreturn__)) void abort(void)
{
#ifdef __THEOS_KERNEL
    klog(FATAL, "Kernel panic: abort()\r\n");
#endif

    while (true) { }
    __builtin_unreachable();
}