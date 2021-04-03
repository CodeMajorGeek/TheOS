#ifndef _KERNEL_KMEM_H
#define _KERNEL_KMEM_H

#include <Boot/multiboot.h>

#include <Kernel/vmem.h>
#include <Kernel/kmem.h>
#include <Kernel/page.h>

#include <stdint.h>
#include <stdbool.h>

extern uint32_t kmem_addr;
extern vmem_heap_t* kmem_heap;

uint32_t kmalloc_i(uint32_t, bool, uint32_t*);

uint32_t kmalloc_a(uint32_t);
uint32_t kmalloc_p(uint32_t, uint32_t*);
uint32_t kmalloc_ap(uint32_t, uint32_t*);
uint32_t kmalloc(uint32_t);

void kfree(uint32_t);
uint32_t kmem_total(void);
void kmem_init(multiboot_info_t*);

#endif