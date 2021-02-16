#ifndef THEOS_TTY_H
#define THEOS_TTY_H

#include <kernel/vga.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define DISPLAY_ADDRESS 0xB8000

void tty_init(void);
void tty_set_color(uint8_t);
void tty_put_entry_at(unsigned char, uint8_t, size_t, size_t);
void tty_putc(char);
void tty_write(const char*, size_t);
void tty_puts(const char*);

#endif