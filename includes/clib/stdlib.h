#ifndef _CLIB_STDLIB_H
#define _CLIB_STDLIB_H

#include <stdbool.h>
#include <stdio.h>

#ifdef __THEOS_KERNEL
#include <kernel/logger.h>
#endif

__attribute__((__noreturn__)) void abort(void);

#endif