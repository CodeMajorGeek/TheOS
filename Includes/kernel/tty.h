#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <kernel/vga.h>
#include <kernel/io.h>

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#define DISPLAY_ADDRESS 0xB8000

#define CURSOR_CTRL 0x3D4
#define CURSOR_DATA (CURSOR_CTRL + 1)

void tty_init(void);
void tty_set_color(uint8_t);
void tty_put_entry_at(unsigned char, uint8_t, size_t, size_t);
void tty_putc(char);
void tty_write(const char*, size_t);
void tty_puts(const char*);
void tty_clear(void);
void tty_enable_cursor(bool);
void tty_update_cursor(uint8_t, uint8_t);

#endif