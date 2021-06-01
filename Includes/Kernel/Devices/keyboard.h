#ifndef _KERNEL_KEYBOARD_H
#define _KERNEL_KEYBOARD_H

#include <Kernel/CPU/isr.h>
#include <Kernel/CPU/io.h>

#include <stdbool.h>
#include <stdio.h>

#define PORT_SCANCODE   0x60

void keyboard_init(void);

uint8_t keyboard_wait_scancode(void);
char keyboard_wait_ASCII(void);

#endif