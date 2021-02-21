.set ALIGN,     1 << 0
.set MEMINFO,   1 << 1
.set AOUTK,     1 << 16
.set FLAGS,     ALIGN | MEMINFO
.set MAGIC,     0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB

stack_top:
.section .text
.globl _start
.type _start, @function

.extern k_entry

_start:
    mov $stack_top, %esp
    push %esp
    push %ebx
    call k_entry
    cli

1:
    hlt
    jmp 1b

.size _start, . -_start
