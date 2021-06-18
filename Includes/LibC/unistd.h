#ifndef _CLIB_UNISTD_H
#define _CLIB_UNISTD_H

#include <stdio.h>
#include <stddef.h>
#include <sys/syscall.h>

#define STDIN   0

#define EBADF   3450

int execve(const char*, const char* [], const char* []);
ssize_t read(int, void*, size_t);

#endif