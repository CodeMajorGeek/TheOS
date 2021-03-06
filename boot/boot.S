.code32
.text

start:
_start:
    jmp multiboot_entry

.set ALIGN,     1 << 0
.set MEMINFO,   1 << 1
.set FLAGS,     ALIGN | MEMINFO
.set MAGIC,     0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
multiboot_header:
.long MAGIC
.long FLAGS
.long CHECKSUM
.long multiboot_header
.long _start
.long stack_top
.long _end
.long multiboot_entry
.long 0
.long 1024
.long 768
.long 32

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.globl _start
.type _start, @function

.extern k_entry

multiboot_entry:
    mov $stack_top, %esp

    pushl $0
    popf

    pushl %esp
    pushl %ebx
    pushl %eax
    call k_entry
    cli

1:
    hlt
    jmp 1b

_end:

