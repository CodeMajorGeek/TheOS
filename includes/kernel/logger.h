#ifndef THEOS_LOGGER_H
#define THEOS_LOGGER_H

#include <kernel/serial.h>

#include <stdint.h>

#define SERIAL COM1

/* Level priority of logging */
#define INFO    0
#define DEBUG   1
#define WARN    2
#define ERROR   3

void logger_init(void);
void klog(uint8_t, const char*);

#endif