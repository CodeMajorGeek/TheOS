#include <boot/boot.h>

.code16
.globl _start

_start:
    xor %ax, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss

    mov $STACK_POINTER, %esp

    call print
    .string "[bootloader] Initializing...\r\n"

    mov $(LOADER_KERN_BASE / 0x10), %ax
    mov %ax, %es
    xor %bx, %bx

    call load_setup
    call load_efi

load_setup:
    pusha

    call print
    .string "[bootloader] Configurating boot sectors...\r\n"

    mov $0x0220, %ax # AH->sr       AL->nrSec
    mov $0x0002, %cx # CH->cyl      CL->sec
    mov $0x0000, %dh # DH->head     DL->drive

    call print
    .string "[bootloader] Reseting har drive...\r\n"

    int $0x13
    or %ah, %ah

    jz popa_ret

    call print
    .string "[bootloader] Loaded !\r\n"

hlt:
    jmp hlt

load_efi:
    mov %es:0x18, %dx
    mov %dx, addr_bootable
    movw $(LOADER_KERN_BASE / 0x10), addr_bootable + 2

    call print
    .string "[bootloader] EFI address loaded !\r\n"

    ljmp *addr_bootable
    ret

print:
    xchg %si, %ss:(%esp)
    push %ax

print_next_char:
    mov %cs:(%si), %al
    inc %si
    test %al, %al
    jz 1f
    call put_char
    jmp print_next_char

1:
    pop %ax
    xchg %si, %ss:(%esp)
    ret

put_char:
    pusha
    sub %bh, %bh
    mov $0x0e, %ah
    int $0x10

popa_ret:
    popa
    ret

addr_bootable:
    .fill 510 - (.-_start), 1, 0 # Fill with 0 to make it 510 bytes long
    .word 0xaa55 # Tell the BIOS that it is bootable