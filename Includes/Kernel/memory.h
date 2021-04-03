#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <Kernel/io.h>

#include <stdint.h>

#define CMOS_REGISTER_IN        0x70
#define CMOS_REGISTER_OUT       0x71

enum cmos_time
{
    CMOS_SECONDS = 0x00,
    CMOS_MINUTES = 0x02,
    CMOS_HOURS = 0x04,
    CMOS_WEEKDAY = 0x06,
    CMOS_DAY = 0x07,
    CMOS_MONTH = 0x08,
    CMOS_YEAR = 0x09,
    CMOS_CENTURY = 0x32
};

#define CMOS_SIZE_SEGMENT       0x30

short memory_CMOS_get_size(void);

uint8_t memory_CMOS_get_time(enum cmos_time);

#endif