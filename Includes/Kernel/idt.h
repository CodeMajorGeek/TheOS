#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#include <Kernel/isr.h>

#include <stdint.h>

#define KERNEL_CS 0x08

#define MAX_ENTRIES 256

typedef struct 
{
    uint16_t base_low;          // Low bits of the address to jump to when interrupt fires.
    uint16_t segment_selector;  // Code segment selector in GDT.
    uint8_t  reserved;          // Always 0.
    uint8_t  type;              // Type: trap, interrupt gates.
    uint16_t base_high;         // High bits of the address to jump to.
} __attribute__((__packed__)) IDT_t;

typedef struct
{
    uint16_t limit_size;        // Limit size of all IDT segements.
    uint32_t base_address;   // Base address of the first IDT segment.
} __attribute__((__packed__)) IDT_PTR_t;

extern void load_idt(uint32_t);

void idt_set_gate(int, uint32_t);
void idt_set(void);

#endif