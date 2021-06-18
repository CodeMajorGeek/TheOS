#ifndef _CLIB_KEYBOARD_H
#define _CLIB_KEYBOARD_H

#include <Kernel/Devices/keyboard.h>

#include <stdint.h>

char scancode_to_ASCII(uint8_t);

#endif