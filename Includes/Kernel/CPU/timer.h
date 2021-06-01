#ifndef _KERNEL_TIMER_H
#define _KERNEL_TIMER_H

#include <Kernel/Process/task.h>
#include <Kernel/CPU/isr.h>
#include <Kernel/CPU/io.h>

#include <stdint.h>

#define BASE_FREQUENCY 1193180

void timer_init(void);

void timer_phase(uint16_t);

void sleep(uint32_t);

#endif