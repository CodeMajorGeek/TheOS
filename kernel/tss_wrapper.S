.globl load_tss
.globl tss_switch_usermode

.extern test_usermode

load_tss:
    movw $0x2B, %ax # 8 bytes long index 5 in GDT.
    ltr %ax
    ret

tss_switch_usermode:
    cli
    movw $0x23, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs # SS handler by iret.

    mov %esp, %eax

    pushl $0x23 # User data segment with 2 lo bits set for ring 3.
    pushl %eax  # Push current esp for iret stack frame.
    pushf
    push $0x1B # User code segment with 2 lo bits set for ring 3.
    push $test_usermode
    iret