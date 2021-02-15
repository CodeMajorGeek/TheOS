#include <boot/boot.h>

.section .start
.code32
.globl start
.extern k_entry

start:
1:
    mov $SEL_KDSEG, %ax

    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    addl $LOADER_PHYS_BASE, %esp
    movl $0, %ebp

    jmp k_entry

1:
    jmp 1b