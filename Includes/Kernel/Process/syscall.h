#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#include <Kernel/CPU/system.h>
#include <Kernel/Process/task.h>

#include <stdio.h>

#define MAX_SYSCALLS 3

void syscall_handler(registers_t*);

#endif