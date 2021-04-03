#include <Kernel/Memory/memory.h>

short memory_CMOS_get_size(void)
{
    io_outb(CMOS_REGISTER_IN, CMOS_SIZE_SEGMENT);
    uint8_t lo = io_inb(CMOS_REGISTER_OUT);
    io_outb(CMOS_REGISTER_IN, CMOS_SIZE_SEGMENT + 1);
    uint8_t hi = io_inb(CMOS_REGISTER_OUT);

    return lo | hi << 8;
}

uint8_t memory_CMOS_get_time(enum cmos_time time)
{
    io_outb(CMOS_REGISTER_IN, time);
    return io_inb(CMOS_REGISTER_OUT);
}