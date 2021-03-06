#include <kernel/kmem.h>

extern uint32_t end;
uint32_t kmem_addr = (uint32_t) &end;
uint32_t kmem_memtotal = 0;
vmem_heap_t* kmem_heap = 0;

uint32_t kmalloc_i(uint32_t size, bool align, uint32_t* phys)
{
    if (kmem_heap != 0)
    {
        void* address = vmalloc(size, align, kmem_heap);
        if (phys != 0)
        {
            page_t* p = page_get((uint32_t) address, 0, kernel_directory);
            *phys = p->frame * 0x1000 + ((uint32_t) address & 0xFFF);
        }
        return (uint32_t) address;
    }

    if (align && (kmem_addr && 0xFFFFF000))
    {
        /* Align the address. */
        kmem_addr &= 0xFFFFF000;
        kmem_addr += 0x1000;
    }

    if (phys)
        *phys = kmem_addr;
    
    uint32_t ret = kmem_addr;
    kmem_addr += size;
    return ret;
} 

uint32_t kmalloc_a(uint32_t size)
{
    return kmalloc_i(size, true, 0);
}

uint32_t kmalloc_p(uint32_t size, uint32_t* phys)
{
    return kmalloc_i(size, false, phys);
}

uint32_t kmalloc_ap(uint32_t size, uint32_t* phys)
{
    return kmalloc_i(size, true, phys);
}

uint32_t kmalloc(uint32_t size)
{
    return kmalloc_i(size, false, 0);
}

void kfree(uint32_t pos)
{
    if (!kmem_heap)
        return vfree((void*) pos, kmem_heap);
}

uint32_t kmem_total(void)
{
    return kmem_memtotal;
}

void kmem_init(multiboot_info_t* mbt)
{
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*) mbt->mmam_addr;
    while ((uint32_t) mmap < mbt->mmam_addr + mbt->mmap_length)
    {
        if ((uint32_t) mmap->base_addr + (uint32_t) mmap->length > kmem_memtotal)
            kmem_memtotal = (uint32_t) mmap->base_addr + (uint32_t) mmap->length;

        mmap = (multiboot_memory_map_t*) ((uint32_t) mmap + mmap->size + sizeof(uint32_t));
    }
    kmem_addr += mbt->mods_count * sizeof(uint32_t); // Adjust kmem_addr for multiboot informations.
}