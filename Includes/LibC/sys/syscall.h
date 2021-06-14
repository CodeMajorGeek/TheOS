#ifndef _CLIB_SYSCALL_H
#define _CLIB_SYSCALL_H

#include <stdint.h>
#include <stdio.h>

int syscall(int, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

int $sys$putc(int);
int $sys$puts(const char*);

int $sys$getc(void);

int $sys$fork(void);

#endif