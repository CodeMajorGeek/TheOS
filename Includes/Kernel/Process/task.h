#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

#include <Kernel/CPU/tss.h>
#include <Kernel/Memory/page.h>
#include <Kernel/Memory/frame.h>
#include <Kernel/Utils/logger.h>

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define KERNEL_STACK_SIZE 2048 // Use a 2kb kernel stack.

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
extern void perform_task_switch(uint32_t, uint32_t, uint32_t, uint32_t);

void task_init(uint32_t);
void task_switch(void);
void task_move_stack(void*, uint32_t);

int task_fork(void);
int task_getpid(void);
void task_switch(void);
void switch_to_user_mode(void);

#endif