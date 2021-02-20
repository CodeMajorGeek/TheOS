#ifndef _BOOT_KMEM_H
#define _BOOT_KMEM_H

#include <stdint.h>

typedef struct
{
    uint32_t magic, flags, checksum;
    uint32_t header_addr;
    uint32_t load_addr, load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} __attribute__((__packed__)) multiboot_header_t;

typedef struct
{
    uint32_t tab_size;
    uint32_t str_size;
    uint32_t addr;
    uint32_t reserved;
} __attribute__((__packed__)) aout_symbol_table_t;

typedef struct
{
    uint32_t flags;
    uint32_t mem_lower, mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count, mods_addr;
    aout_symbol_table_t symbols;
    uint32_t mmap_length;
    uint32_t mmam_addr;
} __attribute__((__packed__)) multiboot_info_t;

typedef struct
{
    uint32_t size;
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
} __attribute__((__packed__)) multiboot_memory_map_t;

#endif