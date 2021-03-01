#ifndef _CLIB_STDIO_H
#define _CLIB_STDIO_H

#ifdef __THEOS_KERNEL
#include <kernel/tty.h>
#else
#include <sys/syscall.h>
#endif

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#define EOF (-1)

#define BINARY      2
#define DECIMAL     10
#define HEXADECIMAL 16

int putc(int);
static bool print(const char*, size_t, bool);

int printf(const char* __restrict, ...);
int puts(const char*);

char* itoa(int, char*, size_t, unsigned int);
int sprintf(char*, const char*, ...);

#endif