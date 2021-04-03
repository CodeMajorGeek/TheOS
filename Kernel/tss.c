#include <Kernel/tss.h>

TSS_t tss_entry;

void tss_init(uint8_t index, uint16_t ss0, uint32_t esp0)
{
    uint32_t base = (uint32_t) &tss_entry;
    uint32_t size = base + sizeof(TSS_t);

    gdt_set_gate(index, base, size, 0xE9, 0x00);

    memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;

    tss_entry.cs = 0x0B;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
}

void tss_set_kernel_stack(uint32_t stack)
{
    tss_entry.esp0 = stack;
}