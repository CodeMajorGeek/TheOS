#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#include <Kernel/CPU/system.h>

#include <stdio.h>

#define MAX_SYSCALLS 1

void syscall_handler(registers_t*);

#endif