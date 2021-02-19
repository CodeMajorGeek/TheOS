#ifndef _KERNEL_TSS_H
#define _KERNEL_TSS_H

#include <kernel/gdt.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef struct 
{
    uint16_t link, link_reserved;
    uint32_t esp0; 
    uint16_t ss0, ss0_reserved;
    uint32_t esp1;
    uint16_t ss1, ss1_reserved;
    uint32_t esp2;
    uint16_t ss2, ss2_reserved;
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
    uint16_t es, es_reserved;
    uint16_t cs, cs_reserved;
    uint16_t ss, ss_reserved;
    uint16_t ds, ds_reserved;
    uint16_t fs, fs_reserved;
    uint16_t gs, gs_reserved;
    uint16_t ldtr, ldtr_reserved;
    uint16_t iopb_offset_reserved, iopb_offset;
} __attribute__((__packed__)) TSS_t;

extern void load_tss(void);
extern void tss_switch_usermode(void);

void tss_init(uint8_t, uint16_t, uint16_t);

#endif