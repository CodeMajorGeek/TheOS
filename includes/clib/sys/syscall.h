#ifndef _CLIB_SYSCALL_H
#define _CLIB_SYSCALL_H

#include <stdint.h>

int syscall(int, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

#endif