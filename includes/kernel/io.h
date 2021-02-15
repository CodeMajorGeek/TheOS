#ifndef THEOS_IO_H
#define THEOS_IO_H

#include <stdint.h>

void io_outb(uint16_t, uint8_t);

uint8_t io_inb(uint16_t);

#endif