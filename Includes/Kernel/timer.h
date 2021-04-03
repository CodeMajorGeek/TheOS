#ifndef _KERNEL_TIMER_H
#define _KERNEL_TIMER_H

#include <Kernel/task.h>
#include <Kernel/isr.h>
#include <Kernel/io.h>

#include <stdint.h>

#define BASE_FREQUENCY 1193180

void timer_init(void);

void timer_phase(uint16_t);

#endif