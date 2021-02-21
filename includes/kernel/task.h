#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

#include <kernel/tss.h>
#include <kernel/page.h>
#include <kernel/frame.h>
#include <kernel/logger.h>

#include <stdint.h>
#include <string.h>

#define KERNEL_STACK_SIZE 2048

typedef struct task
{
    int id;
    uint32_t esp, ebp;
    uint32_t eip;
    page_directory_t* page_directory;
    uint32_t kernel_stack;
    struct task* next;
} task_t;

extern uint32_t read_eip(void);

void task_init(uint32_t);
void task_switch(void);
void task_move_stack(void*, uint32_t);

int task_fork(void);
int task_getpid(void);

#endif