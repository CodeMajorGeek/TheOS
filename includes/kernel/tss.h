#ifndef _KERNEL_TSS_H
#define _KERNEL_TSS_H

#include <kernel/gdt.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef struct 
{
    uint32_t prev_tss;
    uint32_t esp0; 
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_offset;
} __attribute__((__packed__)) TSS_t;

extern void load_tss(void);
extern void tss_switch_usermode(void);

void tss_init(uint8_t, uint16_t, uint32_t);
void tss_set_kernel_stack(uint32_t);
void tss_switch(void);

#endif