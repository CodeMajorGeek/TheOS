#ifndef _KERNEL_KEYBOARD_H
#define _KERNEL_KEYBOARD_H

#include <Kernel/CPU/isr.h>
#include <Kernel/CPU/io.h>

#include <stdbool.h>
#include <stdio.h>

#define PORT_STATUS             0x64
#define PORT_DATA               0x60

#define SCANCODE_BUFFER_SIZE    8

void keyboard_wait_ack(void);
void keyboard_update_leds(uint8_t);
void keyboard_init(void);

uint8_t keyboard_get_scancode(void);

#endif