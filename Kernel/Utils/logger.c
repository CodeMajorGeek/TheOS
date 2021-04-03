#include <Kernel/Utils/logger.h>

void logger_init(void)
{
#ifdef __USE_QEMU
    serial_init(SERIAL);
#endif
}

void log(uint8_t level, const char* str)
{
#ifdef __USE_QEMU
    serial_puts(SERIAL, str);
#else
    puts(str);
#endif
    
}

void klog(uint8_t level, const char* str)
{
#ifndef __DEBUG
    if (level == DEBUG)
        return;
#endif

    switch (level)
    {
    case 0:
        log(level, "[INFO] ");
        break;
    case 1:
        log(level, "[DEBUG] ");
        break;
    case 2:
        log(level, "[WARN] ");
        break;
    case 3:
        log(level, "[ERROR] ");
        break;
    default:
        log(level, "[FATAL] ");
        break;
    }
    log(level, str);
    log(level, "\r\n");
}