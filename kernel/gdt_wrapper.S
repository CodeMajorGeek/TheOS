.set CR0_PE, 0x00000001 # Protection Enable.
.set CR0_EM, 0x00000004 # Emulation (Floating-point).
.set CR0_PG, 0x80000000 # Paging.
.set CR0_WP, 0x00010000 # Write-Protect enable in kernel mode.

.set CR0_FLAG, CR0_PE | CR0_EM | CR0_PG | CR0_WP

.section .text
.globl load_gdt

load_gdt:
    mov 4(%esp), %eax
    lgdt (%eax)

    mov $0x10, %eax
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %fs
    mov %eax, %gs
    mov %eax, %ss
    jmp $0x8, $.long_jump

.long_jump:
    ret