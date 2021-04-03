#ifndef _KERNEL_PAGE_H
#define _KERNEL_PAGE_H

#include <Kernel/Memory/kmem.h>
#include <Kernel/Memory/vmem.h>
#include <Kernel/Memory/frame.h>
#include <Kernel/CPU/system.h>
#include <Kernel/Utils/logger.h>
#include <Kernel/FileSystem/vfs.h>

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    page_t pages[1024];
} page_table_t;

typedef struct {
    page_table_t* tables[1024]; // Array of pointers to page tables.
    uint32_t tables_phys[1024]; // Array of physical page location for CR3.
    uint32_t phys_address;      // Physical address og table_phys.
} page_directory_t;

extern page_directory_t* current_directory;
extern page_directory_t* kernel_directory;

extern void copy_page_physical(uint32_t, uint32_t);

page_directory_t* clone_directory(page_directory_t*);

void page_fault_handler(registers_t*);

void page_init(void);
void page_switch(page_directory_t*);
page_t* page_get(uint32_t, bool, page_directory_t*);

#endif