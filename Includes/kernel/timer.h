#ifndef _KERNEL_TIMER_H
#define _KERNEL_TIMER_H

#include <kernel/task.h>
#include <kernel/isr.h>
#include <kernel/io.h>

#include <stdint.h>

#define BASE_FREQUENCY 1193180

void timer_init(void);

void timer_phase(uint16_t);

#endif