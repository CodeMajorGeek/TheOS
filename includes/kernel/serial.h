#ifndef THEOS_SERIAL_H
#define THEOS_SERIAL_H

#include <kernel/io.h>

#include <stdint.h>
#include <stdbool.h>

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

bool serial_init(uint16_t);

bool serial_received(uint16_t);
char serial_read(uint16_t);

bool serial_is_transmit_empty(uint16_t);
void serial_write(uint16_t, char);
void serial_puts(uint16_t, const char*);

#endif