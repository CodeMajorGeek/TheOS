#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

#include <stdint.h>

void io_outb(uint16_t, uint8_t);

uint8_t io_inb(uint16_t);
uint32_t io_inl(uint16_t);

void io_outsw(uint32_t, uint32_t, uint32_t);

void io_insl(uint32_t, uint32_t*, uint32_t);
void io_insw(uint32_t, uint32_t, uint32_t);

#endif