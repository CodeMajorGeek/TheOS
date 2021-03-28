#ifndef _KERNEL_VMEM_H
#define _KERNEL_VMEM_H

#include <kernel/ordered_array.h>
#include <kernel/frame.h>
#include <kernel/system.h>
#include <kernel/kmem.h>

#include <stdbool.h>
#include <stdint.h>

#define VMEM_START          0xC0000000
#define VMEM_INITIAL_SIZE   0x100000
#define VMEM_INDEX_SIZE     0x20000
#define VMEM_MAGIC          0x123890AB
#define VMEM_MIN_SIZE       0x70000

vmem_heap_t* vcreate_heap(uint32_t, uint32_t, uint32_t, bool, bool);
void* vmalloc(uint32_t, bool, vmem_heap_t*);
void vfree(void*, vmem_heap_t*);


#endif