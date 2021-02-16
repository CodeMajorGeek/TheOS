#ifndef _CLIB_STDIO_H
#define _CLIB_STDIO_H

#ifdef __THEOS_KERNEL
#include <kernel/tty.h>
#endif

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#define EOF (-1)

int putc(int);
static bool print(const char*, size_t);
int printf(const char* __restrict, ...);
int puts(const char*);

#endif