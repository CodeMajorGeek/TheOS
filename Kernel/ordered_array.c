#include <kernel/ordered_array.h>

bool standard_lessthan_predicate(type_t a, type_t b)
{
    return a < b;
}

ordered_array_t place_ordered_array(void* address, uint32_t max_size, lessthan_predicate_t less_than)
{
    ordered_array_t ret;
    ret.array = (type_t*) address;
    memset(ret.array, 0, max_size * sizeof(type_t));
    ret.size = 0;
    ret.max_size = max_size;
    ret.less_than = less_than;
    return ret;
}

ordered_array_t create_ordered_array(uint32_t max_size, lessthan_predicate_t less_than)
{
    return place_ordered_array((type_t*) kmalloc(max_size * sizeof(type_t)), max_size, less_than);
}

void insert_ordered_array(type_t item, ordered_array_t* array)
{
    uint32_t iterator = 0;
    while (iterator < array->size && array->less_than(array->array[iterator], item))
        iterator++;

    if (iterator == array->size)
        array->array[array->size++] = item;
    else
    {
        type_t tmp = array->array[iterator];
        array->array[iterator] = item;
        while (iterator < array->size)
        {
            iterator++;
            type_t tmp2 = array->array[iterator];
            array->array[iterator] = tmp;
            tmp = tmp2;
        }
        array->size++;
    }
}

type_t lookup_ordered_array(uint32_t index, ordered_array_t* array)
{
    return array->array[index];
}

void remove_orderd_array(uint32_t index, ordered_array_t* array)
{
    while (index < array->size)
    {
        array->array[index] = array->array[index + 1];
        index++;
    }
    array->size--;
}