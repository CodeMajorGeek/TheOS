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
    bool present    : 1;
    bool rw         : 1;
    bool user       : 1;
    bool accessed   : 1;
    bool dirty      : 1;
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
    bool supervisor;
    bool readonly;
} vmem_heap_t;

typedef struct
{
    page_t pages[1024];
} page_table_t;

typedef struct {
    page_table_t* tables[1024]; // Array of pointers to page tables.
    uint32_t tables_phys[1024]; // Array of physical page location for CR3.
    uint32_t phys_address;      // Physical address og table_phys.
} page_directory_t;

#endif