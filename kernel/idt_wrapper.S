.section .text
.align 4

.globl load_idt

load_idt:
    mov 4(%esp), %edx
    lidtl (%edx)
    sti
    ret