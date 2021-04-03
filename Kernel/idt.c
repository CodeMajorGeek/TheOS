#include <Kernel/idt.h>

IDT_t idt_entries[MAX_ENTRIES];
IDT_PTR_t idt_first;

void idt_set_gate(int inum, uint32_t isr)
{
    idt_entries[inum].base_low = isr & 0xFFFF;
    idt_entries[inum].segment_selector = KERNEL_CS;
    idt_entries[inum].reserved = 0;
    idt_entries[inum].type = 0x8E | 0x60;
    idt_entries[inum].base_high = (isr >> 16) & 0xFFFF;
}

void idt_set(void)
{
    idt_first.limit_size = sizeof(idt_entries) - 1;
    idt_first.base_address = (uint32_t) &idt_entries;

    load_idt((uint32_t) &idt_first);
}