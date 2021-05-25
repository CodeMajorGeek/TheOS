#include <Kernel/Memory/page.h>

page_directory_t* current_directory = 0;
page_directory_t* kernel_directory = 0;

static page_table_t* clone_table(page_table_t* src, uint32_t* phys_addr)
{
    /* Make a new page aligned table. */
    page_table_t* table = (page_table_t*) kmalloc_ap(sizeof(page_table_t), phys_addr);
    memset(table, 0, sizeof(page_table_t));

    /* Every entry in the table. */
    for (int i = 0; i < 1024; i++)
    {
        if (!src->pages[i].frame)
            continue;

        frame_alloc(&table->pages[i], false, false);

        /* Clone the flags from the source to destination. */
        table->pages[i].present = src->pages[i].present >= 1;   
        table->pages[i].rw = src->pages[i].rw >= 1; 
        table->pages[i].user = src->pages[i].user >= 1; 
        table->pages[i].accessed = src->pages[i].accessed >= 1; 
        table->pages[i].dirty = src->pages[i].dirty >= 1;

        copy_page_physical(src->pages[i].frame * 0x1000, table->pages[i].frame * 0x1000); // Physically copy the data across
    }
    return table;
}

page_directory_t* clone_directory(page_directory_t* src)
{
    uint32_t phys;
    page_directory_t* dir = (page_directory_t*) kmalloc_ap(sizeof(page_directory_t), &phys);

    memset(dir, 0, sizeof(page_directory_t));
    
    /* Get offset tables_phys from the start of page_directory. */
    uint32_t offset = (uint32_t) dir->tables_phys - (uint32_t) dir;
    dir->phys_address = phys + offset;

    /* Copy each page table. */
    for (int i = 0; i < 1024; i++)
    {
        if (!src->tables[i])
            continue;
        if (kernel_directory->tables[i] == src->tables[i])
        {
            dir->tables[i] = src->tables[i];
            dir->tables_phys[i] = src->tables_phys[i];
        }
        else
        {
            dir->tables[i] = clone_table(src->tables[i], &phys);
            dir->tables_phys[i] = phys | 0x07;
        }
    }
    return dir;
}

void page_fault_handler(registers_t* r)
{
    uint32_t fault_location;
    __asm__ __volatile__("mov %%cr2, %0": "=r"(fault_location));

    bool present  = !(r->err_code & 0x1);
    bool rw       = r->err_code & 0x2;
    bool us       = r->err_code & 0x4;
    bool reserved = r->err_code & 0x8;
    
    // uint8_t id = r->err_code & 0x10;

    /* Output an error message. */
    char sp_buf[256];
    sprintf(sp_buf, "Page Fault at 0x%H:\r\n\tPresent: %b\r\n\tRead-only: %b\r\n\tUser-mode: %b\r\n\tReserved: %b", fault_location, present, rw, us, reserved);
    klog(FATAL, sp_buf);
}

void page_init(void)
{
    uint8_t sp_buffer[256];
    int index = 0;
    uint32_t memend = kmem_total();
    
    /* Set the frames and frame count. */
    klog(INFO, "Initializing physical frames...");
    nframes = memend / 0x1000;
    frames = (uint32_t*) kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));
    sprintf(sp_buffer, "Done (%d).", nframes);
    klog(INFO, sp_buffer);

    /* Initialize the page directory area. */
    klog(INFO, "Initializing memory for page directory...");
    kernel_directory = (page_directory_t*) kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    kernel_directory->phys_address = (uint32_t) kernel_directory->tables_phys;
    memset(sp_buffer, 0, sizeof(sp_buffer));
    sprintf(sp_buffer, "Done at 0x%H.", (uint32_t) kernel_directory);
    klog(INFO, sp_buffer);

    /* Map some pages in the kernel heap area. */
    for (int i = VMEM_START; i < VMEM_START + VMEM_INITIAL_SIZE; i += 0x1000)
        page_get(i, true, kernel_directory);
    
    /* Set the initial state of the page directory. */
    klog(INFO, "Initializing contents of page directory...");
    int i = 0;
    while (i < kmem_addr + 0x1000)
    {
        /* Kernel code is readable but not writtable frome userspace. */
        frame_alloc(page_get(i, true, kernel_directory), false, false);
        i += 0x1000;
    }
    klog(INFO, "Done.");

    /* Allocate those pages mapped earlier. */
    for (i = VMEM_START; i < VMEM_START + VMEM_INITIAL_SIZE; i += 0x1000)
        frame_alloc(page_get(i, true, kernel_directory), false, false);

    /* Enable paging. */
    klog(INFO, "Enabling paging...");
    page_switch(kernel_directory);
    klog(INFO, "Done.");

    /* Initialize kernel virtual memory. */
    klog(INFO, "Creating kernel heap...");
    kmem_heap = vcreate_heap(VMEM_START, VMEM_START + VMEM_INITIAL_SIZE, 0xCFFFF000, false, false);
    klog(INFO, "Done.");

    /* Clone the kernel direcotry. */
    klog(INFO, "Cloning kernel page directory...");
    current_directory = clone_directory(kernel_directory);
    klog(INFO, "Done.");
    klog(INFO, "Switching to cloned directory...");
    page_switch(current_directory);
    klog(INFO, "Done.");
}

void page_switch(page_directory_t* dir)
{
    uint32_t cr0;
    current_directory = dir;

    __asm__ __volatile__("mov %0, %%cr3" :: "r"(dir->phys_address));
    __asm__ __volatile__("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ __volatile__("mov %0, %%cr0":: "r"(cr0));
}

page_t* page_get(uint32_t address, bool make, page_directory_t* dir)
{
    address /= 0x1000; // Turn address into index.

    uint32_t table_index = address / 1024;
    if (dir->tables[table_index]) // Already assigned.
        return &dir->tables[table_index]->pages[address % 1024];
    else if (make)
    {
        uint32_t tmp;
        dir->tables[table_index] = (page_table_t*) kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(dir->tables[table_index], 0, 0x1000);
        dir->tables_phys[table_index] = tmp | 0x7;
        return &dir->tables[table_index]->pages[address % 1024];
    }
    else
        return 0;
}