#ifndef _KERNEL_LOGGER_H
#define _KERNEL_LOGGER_H

#ifdef __USE_QEMU
#include <Kernel/serial.h>
#else
#include <stdio.h>
#endif

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __USE_QEMU
#define SERIAL COM1
#endif

/* Level priority of logging */
#define INFO    0
#define DEBUG   1
#define WARN    2
#define ERROR   3
#define FATAL   4

void logger_init(void);
void log(uint8_t, const char*);
void klog(uint8_t, const char*);

#endif