#include <kernel/gdt.h>

GDT_t gdt_entries[MAX_GDT_DESC];
GDT_PTR_t gdt_first;

void gdt_set_gate(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    /* Setup base address. */
    gdt_entries[index].base_low = (base & 0xFFFF);
    gdt_entries[index].base_middle = (base >> 16) & 0xFF;
    gdt_entries[index].base_high = (base >> 24) & 0xFF;

    /* Setup limits. */
    gdt_entries[index].segment_limit = limit & 0xFFFF;
    gdt_entries[index].granularity = (limit >> 16) & 0x0F;

    /* Setup granularity and access flags. */
    gdt_entries[index].granularity |= (gran & 0xF0);
    gdt_entries[index].access = access;
}

void gdt_init(void)
{
    /* Set null segment */
    gdt_set_gate(NULL_SEGMENT, 0, 0, 0, 0);
    
    /* Set kernel code segment. */
    gdt_set_gate(KERNEL_CODE_SEGMENT, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* Set kernel data segment. */
    gdt_set_gate(KERNEL_DATA_SEGMENT, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* Set user code segment. */
    gdt_set_gate(USER_CODE_SEGMENT, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    /* Set user data segment. */
    gdt_set_gate(USER_DATA_SEGMENT, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    tss_init(TSS_SEGMENT, 0x10, 0x00);

    gdt_first.limit_size = sizeof(gdt_entries) - 1;
    gdt_first.base_address = (uint32_t) gdt_entries;

    load_gdt((uint32_t) &gdt_first);
    load_tss();
}