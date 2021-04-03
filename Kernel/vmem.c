#include <Kernel/vmem.h>

static signed int find_smallest_hole(uint32_t size, bool page_align, vmem_heap_t* heap)
{
    /* Find smallest hole that fit. */
    uint32_t iterator = 0;
    while (iterator < heap->index.size)
    {
        vmem_header_t* header = (vmem_header_t*) lookup_ordered_array(iterator, &heap->index);
        if (page_align > 0)
        {
            uint32_t location = (uint32_t) header;
            signed int offset = 0;
            if ((location + sizeof(vmem_header_t) & 0xFFFFF000) != 0)
                offset = 0x1000 - (location + sizeof(vmem_header_t)) % 0x1000;
                signed int hole_size = (signed int) header->size - offset;
                if (hole_size >= (signed int) size)
                    break;
        }
        else if (header->size >= size)
            break;
        iterator++;
    }
    if (iterator >= heap->index.size)
        return -1; // End and find any hole that fit.
    return iterator;
}

static signed char vmem_header_less_than(void* a, void* b)
{
    return ((vmem_header_t*) a)->size < ((vmem_header_t*) b)->size;
}

vmem_heap_t* vcreate_heap(uint32_t start, uint32_t end_addr, uint32_t max, bool supervisor, bool readonly)
{
    vmem_heap_t* heap = (vmem_heap_t*) kmalloc(sizeof(vmem_heap_t));

    heap->index = place_ordered_array((void*) start, VMEM_INDEX_SIZE, &vmem_header_less_than); // Initialize the index.

    start += sizeof(type_t) * VMEM_INDEX_SIZE; // Shift the start address forward for putting data.

    /* Make sure the start address is page aligned. */
    if (!(start & 0xFFFFF000))
    {
        start &= 0xFFFFF000;
        start += 0x1000;
    }

    /* Write the start, end and max addresses into the heap structure. */
    heap->start_address = start;
    heap->end_address = end_addr;
    heap->max_address = max;
    heap->supervisor = supervisor;
    heap->readonly = readonly;

    /* We start off with one large hole in the index. */
    vmem_header_t* hole = (vmem_header_t*) start;
    hole->size = end_addr - start;
    hole->magic = VMEM_MAGIC;
    hole->is_hole = true;
    insert_ordered_array((type_t) hole, &heap->index);

    return heap;
}

static void expand(uint32_t new_size, vmem_heap_t* heap)
{
    // TODO: Sanity check.

    /* Get the nearest following page boundary. */
    if (!(new_size & 0xFFFFF000))
    {
        new_size &= 0x1000;
        new_size += 0x1000;
    }

    // TODO: Make sure not overreaching this.

    /* Should always be on a page boundary. */
    uint32_t old_size = heap->end_address - heap->start_address;
    uint32_t index = old_size;
    klog(DEBUG, "INFO444");
    while (index < new_size)
    {
        frame_alloc((page_t*) page_get(heap->start_address + index, 1, kernel_directory), heap->supervisor >= 1, !heap->readonly);
        index += 0x1000;
    }
    heap->end_address = heap->start_address + new_size;
}

static uint32_t contract(uint32_t new_size, vmem_heap_t* heap)
{
    // TODO: Sanity check.

    /* Get nearest bounding page boundary. */
    if (new_size & 0x1000)
    {
        new_size &= 0x1000;
        new_size += 0x1000;
    }

    /* Don't contract too far. */
    if (new_size < VMEM_MIN_SIZE)
        new_size = VMEM_MIN_SIZE;

    uint32_t old_size = heap->end_address - heap->start_address;
    uint32_t index = old_size;
    while (new_size < index)
    {
        frame_free((page_t*) page_get(heap->start_address + index, false, kernel_directory));
        index -= 0x1000;
    }
    heap->end_address = heap->start_address + new_size;
    return new_size;
}

void* vmalloc(uint32_t size, bool page_align, vmem_heap_t* heap)
{
    uint32_t new_size = size + sizeof(vmem_header_t) + sizeof(vmem_footer_t);

    signed int iterator = find_smallest_hole(new_size, page_align, heap);
    if (iterator == -1)
    {
        /* Some some previous data. */
        uint32_t old_length = heap->end_address - heap->start_address;
        uint32_t old_end_address = heap->end_address;

        /* We need to allocate some more space. */
        expand(old_length + new_size, heap);
        
        uint32_t new_length = heap->end_address - heap->start_address;

        iterator = 0;
        uint32_t index = -1;
        uint32_t value = 0;
        while (iterator < heap->index.size)
        {
            uint32_t tmp = (uint32_t) lookup_ordered_array(iterator, &heap->index);
            if (tmp > value)
            {
                value = tmp;
                index = iterator;
            }
            iterator++;
        }

        /* When any headers find, need to add one. */
        if (index == -1)
        {
            vmem_header_t* header = (vmem_header_t*) old_end_address;
            header->magic = VMEM_MAGIC;
            header->size = new_length - old_length;
            header->is_hole = true;
            vmem_footer_t* footer = (vmem_footer_t*) (old_end_address + header->size - sizeof(vmem_footer_t));
            footer->magic = VMEM_MAGIC;
            footer->header = header;
            insert_ordered_array((type_t) header, &heap->index);
        }
        else
        {
            /* Adjusting the last header. */
            vmem_header_t* header = lookup_ordered_array(index, &heap->index);
            header->size += new_length - old_length;

            /* Rewrite the footer. */
            vmem_footer_t* footer = (vmem_footer_t*) ((uint32_t) header+ header->size * sizeof(vmem_footer_t));
            footer->header = header;
            footer->magic = VMEM_MAGIC;
        }
    }

    vmem_header_t* orig_hole_header = (vmem_header_t*) lookup_ordered_array(iterator, &heap->index);
    uint32_t orig_hole_pos = (uint32_t) orig_hole_header;
    uint32_t orig_hole_size = orig_hole_header->size;

    /* Split the hole into 2 parts if needed. */
    if (orig_hole_size - new_size < sizeof(vmem_header_t) + sizeof(vmem_footer_t))
    {
        size += orig_hole_size - new_size;
        new_size += orig_hole_size;
    }

    /* New hole in front of block if page-align needed. */
    if (page_align && (orig_hole_size & 0xFFFFF000))
    {
        uint32_t new_location = orig_hole_pos + 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(vmem_header_t);
        vmem_header_t* hole_header = (vmem_header_t*) orig_hole_pos;
        hole_header->size = 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(vmem_header_t);
        hole_header->magic = VMEM_MAGIC;
        hole_header->is_hole = true;
        vmem_footer_t* hole_footer = (vmem_footer_t*) ((uint32_t) new_location - sizeof(vmem_footer_t));
        hole_footer->magic = VMEM_MAGIC;
        hole_footer->header = hole_header;
        orig_hole_pos = new_location;
        orig_hole_size = orig_hole_size - hole_header->size;
    }
    else
    {
        remove_orderd_array(iterator, &heap->index); // Delete if hole isn't used any more.
    }

    /* Overwrite the original header. */
    vmem_header_t* block_header = (vmem_header_t*) orig_hole_pos;
    block_header->magic = VMEM_MAGIC;
    block_header->is_hole = false;
    block_header->size = new_size;

    /* Footer too. */
    vmem_footer_t* block_footer = (vmem_footer_t*) (orig_hole_pos + sizeof(vmem_header_t) + size);
    block_footer->magic = VMEM_MAGIC;
    block_footer->header = block_header;

    /* Maybe need new hole. */
    if (orig_hole_size - new_size > 0)
    {
        vmem_header_t* hole_header = (vmem_header_t*) (orig_hole_pos + sizeof(vmem_header_t) + size + sizeof(vmem_footer_t));
        hole_header->magic = VMEM_MAGIC;
        hole_header->is_hole = true;
        hole_header->size = orig_hole_size - new_size;
        vmem_footer_t* hole_footer = (vmem_footer_t*) ((uint32_t) hole_header + orig_hole_size - new_size - sizeof(vmem_footer_t));
        if ((uint32_t) hole_footer < heap->end_address)
        {
            hole_footer->magic = VMEM_MAGIC;
            hole_footer->header = hole_header;
        }

        insert_ordered_array((type_t) hole_header, &heap->index); // Insert new hole.
    }
    return (type_t)((uint32_t) block_header + sizeof(vmem_header_t));
}

void vfree(void* p, vmem_heap_t* heap)
{
    if(!p)
        return;

    /* Get header and footer from pointer. */
    vmem_header_t* header = (vmem_header_t*) ((uint32_t) p - sizeof(vmem_header_t));
    vmem_footer_t* footer = (vmem_footer_t*) ((uint32_t) header + header->size - sizeof(vmem_footer_t));

    // TODO: Sanity checks.

    header->is_hole = false;

    bool do_add = true;

    /* Unify left. */
    vmem_footer_t* test_footer = (vmem_footer_t*) ((uint32_t) header - sizeof(vmem_footer_t));
    if (test_footer->magic == VMEM_MAGIC && test_footer->header->is_hole)
    {
        uint32_t cache_size = header->size;
        header = test_footer->header;
        footer->header = header;
        header->size += cache_size;
        do_add = false;
    }

    /* Unify right. */
    vmem_header_t* test_header = (vmem_header_t*) ((uint32_t) footer + sizeof(vmem_footer_t));
    if (test_header->magic == VMEM_MAGIC && test_header->is_hole)
    {
        header->size += test_header->size;
        test_footer = (vmem_footer_t*) ((uint32_t) test_header + test_header->size - sizeof(vmem_footer_t));
        footer = test_footer;

        /* Find and remove this kernel from the index. */
        uint32_t iterator = 0;
        while ((iterator < heap->index.size) && (lookup_ordered_array(iterator, &heap->index) != (type_t) test_header))
            iterator++;

        // TODO: Make sure item is found.

        remove_orderd_array(iterator, &heap->index); // Remove it.
    }

    /* If the footer location is the end address, we can contract. */
    if ((uint32_t) footer + sizeof(vmem_footer_t) == heap->end_address)
    {
        uint32_t old_length = heap->end_address - heap->start_address;
        uint32_t new_length = contract((uint32_t) header - heap->start_address, heap);

        /* Check how big it will be after resizing. */
        if (header->size - (old_length - new_length) > 0)
        {
            /* Resize. */
            header->size -= old_length - new_length;
            footer = (vmem_footer_t*) ((uint32_t) header + header->size - sizeof(vmem_footer_t));
            footer->magic = VMEM_MAGIC;
            footer->header = header;
        }
        else
        {
            /* Remove it from the index. */
            uint32_t iterator = 0;
            while ((iterator < heap->index.size) && (lookup_ordered_array(iterator, &heap->index) != (type_t) test_header))
                iterator++;

            if (iterator < heap->index.size)
                remove_orderd_array(iterator, &heap->index);
        }
    }

    if (do_add)
        insert_ordered_array((type_t) header, &heap->index);
}