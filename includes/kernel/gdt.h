#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#include <stdint.h>

#define MAX_GDT_DESC    3

#define NULL_SEGMENT    0
#define CODE_SEGMENT    1
#define DATA_SEGMENT    2

typedef struct
{
    uint16_t segment_limit;     // Segment limit first 0-15 bits.
    uint16_t base_low;          // Base first 0-15 bits.
    uint8_t base_middle;        // Base 16-23 bits.
    uint8_t access;             // Access byte.
    uint8_t granularity;        // High 4 bits (flags), low 4 bits (limit 4 last bits) (limit is 20 bit wide).
    uint8_t base_high;          // Base 24-32 bits.
} __attribute__((__packed__)) GDT_t;

typedef struct
{
    uint16_t limit_size;        // Limit size of all GDT segments.
    uint32_t base_address;        // Base address of the first GDT segment.
} __attribute__((__packed__)) GDT_PTR_t;

extern void load_gdt(uint32_t);

void gdt_init(void);

#endif