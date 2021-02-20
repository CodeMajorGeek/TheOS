#ifndef _KERNEL_SYSTEM_H
#define _KERNEL_SYSTEM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;    // Pushed by "pusha".
    uint32_t int_no, err_code;                          // Interrupt number and error code.
    uint32_t eip, cs, eflags, useresp, ss;              // Pushed by the processor automatically.
} registers_t;

typedef void (*isr_t)(registers_t*);

typedef struct
{
    uint32_t present    : 1;
    uint32_t rw         : 1;
    uint32_t user       : 1;
    uint32_t accessed   : 1;
    uint32_t dirty      : 1;
    uint32_t unused     : 7;
    uint32_t frame      : 20;
} page_t;

typedef void* type_t; // Any type can be stored when can be casted with void*.
typedef signed char (*lessthan_predicate_t)(type_t, type_t); // Should return nonzero if the 1st arg is < than the 2nd, else it should return zero.

typedef struct
{
    type_t* array;
    uint32_t size;
    uint32_t max_size;
    lessthan_predicate_t less_than;
} ordered_array_t;

typedef struct
{
    uint32_t magic;     // Magic number, used for error checking and identification.
    bool is_hole;
    uint32_t size;      // Size of the block.
} vmem_header_t;

typedef struct
{
    uint32_t magic;         // Same as header's magic.
    vmem_header_t* header;  // Pointer to the block header.
} vmem_footer_t;

typedef struct
{
    ordered_array_t index;
    uint32_t start_address;
    uint32_t end_address;
    uint32_t max_address;
    uint8_t supervisor;
    uint8_t readonly;
} vmem_heap_t;

#endif