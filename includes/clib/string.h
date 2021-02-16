#ifndef _CLIB_STRING_H
#define _CLIB_STRING_H

#include <stddef.h>

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void *, const void *, size_t);
size_t strlen(const char*);

void* memset(void*, int, size_t);
void* memsetw(void*, int, size_t);

#endif