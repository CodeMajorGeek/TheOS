#ifndef TheOS_STDINT_H
#define TheOS_STDINT_H

#define FALSE 0
#define TRUE 1

#define false FALSE
#define true TRUE

#define NULL 0

typedef int bool;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef unsigned int uint8_t __attribute__((__mode__(__QI__)));
typedef unsigned int uint16_t __attribute__((__mode__(__HI__)));

#endif