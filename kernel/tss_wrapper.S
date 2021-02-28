.globl load_tss

.extern test_usermode

load_tss:
    movw $0x2B, %ax # 8 bytes long index 5 in GDT.
    ltr %ax
    ret