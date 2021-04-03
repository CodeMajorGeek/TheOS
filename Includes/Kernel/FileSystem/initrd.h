#ifndef _KERNEL_INITRD_H
#define _KERNEL_INITRD_H

#include <Kernel/FileSystem/vfs.h>
#include <Kernel/Memory/kmem.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    uint32_t nfiles;
} initrd_header_t;

typedef struct
{
    uint8_t magic;
    char name[64];
    uint32_t offset;
    uint32_t length;
} initrd_file_header_t;

fs_node_t* initrd_init(uint32_t);

#endif