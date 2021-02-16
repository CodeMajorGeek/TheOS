#include <kernel/logger.h>

void logger_init(void)
{
    serial_init(SERIAL);
}

void klog(uint8_t level, const char* str)
{
    switch (level)
    {
    case 0:
        serial_puts(SERIAL, "[INFO] ");
        break;
    case 1:
        serial_puts(SERIAL, "[DEBUG] ");
        break;
    case 2:
        serial_puts(SERIAL, "[WARN] ");
        break;
    case 3:
        serial_puts(SERIAL, "[ERROR] ");
        break;
    default:
        serial_puts(SERIAL, "[FATAL] ");
        break;
    }
    serial_puts(SERIAL, str);
}