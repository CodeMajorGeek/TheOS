#ifndef _KERNEL_ELF_H
#define _KERNEL_ELF_H

#include <Kernel/Utils/logger.h>
#include <Kernel/Memory/kmem.h>

#include <stdbool.h>
#include <stdint.h>

#define MAGIC               0x464C457F

#define ARCH_x86            1
#define ARCH_x86_64         2

#define TYPE_RELOCATBLE     1
#define TYPE_EXECUTABLE     2
#define TYPE_SHARED         3
#define TYPE_CORE           4

#define OPSET_UKNOWN        0
#define OPSET SPARC         2
#define OPSET_x86           3
#define OPSET_MIPS          8
#define OPSET_POWERPC       0x14
#define OPSET_ARM           0x28
#define OPSET SUPERH        0x2A
#define OPSET_IA_64         0x32
#define OPSET_x86_64        0x3E
#define OPSET_AARCH64       0xB7

#define SEGMENT_NULL        0
#define SEGMENT_LOAD        1
#define SEGMENT_DYNAMIC     2
#define SEGMENT_INTERP      3
#define SEGMENT_NOTE        4

#define PROGRAM_FLAG_EXECUTABLE 1
#define PROGRAM_FLAG_WRITABLE   2
#define PROGRAM_FLAG_READABLE   3

typedef struct
{
    uint32_t magic;
    uint8_t architecture;
    uint8_t endian;
    uint8_t header_version;
    uint8_t OSABI;
    uint16_t unused;
    uint16_t type;
    uint16_t instruction_set;
    uint32_t version;
    uint32_t program_entry;
    uint32_t program_header;
    uint32_t section_header;
    uint32_t flags;
    uint16_t header_size;
    uint16_t program_entry_header_size;
    uint16_t program_entry_header_numbers;
    uint16_t sections_entry_header_size;
    uint16_t sections_entry_header_numbers;
    uint16_t section_header_index;
} ELF_header_t;

typedef struct
{
    uint32_t type;
    uint32_t data_offset;
    uint32_t virtual_memory_offset;
    uint32_t unused;
    uint32_t file_size;
    uint32_t memory_size;
    uint32_t flags;
    uint32_t alignment;
} ELF_program_header_t;

bool elf_load_executable(uint8_t*);

#endif
