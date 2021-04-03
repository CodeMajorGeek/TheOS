#ifndef _KERNEL_ORDERED_ARRAY_H
#define _KERNEL_ORDERED_ARRAY_H

#include <Kernel/kmem.h>
#include <Kernel/system.h>

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

bool standard_lessthan_predicate(type_t, type_t);

/* Create an ordered array. */
ordered_array_t place_ordered_array(void*, uint32_t, lessthan_predicate_t);
ordered_array_t create_ordered_array(uint32_t, lessthan_predicate_t);

void insert_ordered_array(type_t, ordered_array_t*); // Add item into the array.
type_t lookup_ordered_array(uint32_t, ordered_array_t*); // Lookup item at index.
void remove_orderd_array(uint32_t, ordered_array_t*); // Delete item at index.

#endif