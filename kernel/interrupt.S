.section .text

.extern isr_handler
.extern irq_handler

# ISRs.
.globl isr0
.globl isr1
.globl isr2
.globl isr3
.globl isr4
.globl isr5
.globl isr6
.globl isr7
.globl isr8
.globl isr9
.globl isr10
.globl isr11
.globl isr12
.globl isr13
.globl isr14
.globl isr15
.globl isr16
.globl isr17
.globl isr18
.globl isr19
.globl isr20
.globl isr21
.globl isr22
.globl isr23
.globl isr24
.globl isr25
.globl isr26
.globl isr27
.globl isr28
.globl isr29
.globl isr30
.globl isr31

# IRQs.
.globl irq0
.globl irq1
.globl irq2
.globl irq3
.globl irq4
.globl irq5
.globl irq6
.globl irq7
.globl irq8
.globl irq9
.globl irq10
.globl irq11
.globl irq12
.globl irq13
.globl irq14
.globl irq15

isr_common_stub:
    pusha
    mov %ds, %ax
    push %eax
    mov $0x10, %ax # Kernel data segment descriptor.
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    call isr_handler

    pop %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    popa
    add $8, %esp
    sti
    iret

irq_common_stub:
    pusha
    mov %ds, %ax
    push %eax
    mov $0x10, %ax # Kernel data segment descriptor.
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    call irq_handler

    pop %ebx
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    popa
    add $8, %esp
    sti
    iret

# 0: Divide By Zero Exception.
isr0:
    cli
    push $0
    push $0
    jmp isr_common_stub

# 1: Debug Exception.
isr1:
    cli
    push $0
    push $1
    jmp isr_common_stub

# 2: Not Maskable Interrupt Exception.
isr2:
    cli
    push $0
    push $2
    jmp isr_common_stub

# 3: Int 3 Exception.
isr3:
    cli
    push $0
    push $3
    jmp isr_common_stub

# 4: INTO Exception.
isr4:
    cli
    push $0
    push $4
    jmp isr_common_stub

# 5: Out of Bounds Exception.
isr5:
    cli
    push $0
    push $5
    jmp isr_common_stub

# 6: Invalid Opcode Exception.
isr6:
    cli
    push $0
    push $6
    jmp isr_common_stub

# 7: Coprocessor Not Available Exception.
isr7:
    cli
    push $0
    push $7
    jmp isr_common_stub

# 8: Double Fault Exception (with error code).
isr8:
    cli
    push $8
    jmp isr_common_stub

# 9: Coprocessor Segment Overrun Excpetion.
isr9:
    cli
    push $0
    push $9
    jmp isr_common_stub

# 10: Bad TSS Exception (With Error Code).
isr10:
    cli
    push $10
    jmp isr_common_stub

# 11: Stack No Present Exception (with error code).
isr11:
    cli
    push $11
    jmp isr_common_stub

# 12: Stack Fault Exception (with error code).
isr12:
    cli
    push $12
    jmp isr_common_stub

# 13: General Protection Fault Exception (with error code).
isr13:
    cli
    push $13
    jmp isr_common_stub

# 14: Page Fault Exception (with error code).
isr14:
    cli
    push $14
    jmp isr_common_stub

# 15: Reserved Exception
isr15:
    cli
    push $0
    push $15
    jmp isr_common_stub

# 16: Floating Point Exception.
isr16:
    cli
    push $0
    push $16
    jmp isr_common_stub

# 17: Alignment Check Exception.
isr17:
    cli
    push $0
    push $17
    jmp isr_common_stub

# 18: Machine Check Exception.
isr18:
    cli
    push $0
    push $18
    jmp isr_common_stub

# 19: Reserved
isr19:
    cli
    push $0
    push $19
    jmp isr_common_stub

# 20: Reserved
isr20:
    cli
    push $0
    push $20
    jmp isr_common_stub

# 21: Reserved
isr21:
    cli
    push $0
    push $21
    jmp isr_common_stub

# 22: Reserved
isr22:
    cli
    push $0
    push $22
    jmp isr_common_stub

# 23: Reserved
isr23:
    cli
    push $0
    push $23
    jmp isr_common_stub

# 24: Reserved
isr24:
    cli
    push $0
    push $24
    jmp isr_common_stub

# 25: Reserved
isr25:
    cli
    push $0
    push $25
    jmp isr_common_stub

# 26: Reserved
isr26:
    cli
    push $0
    push $26
    jmp isr_common_stub

# 27: Reserved
isr27:
    cli
    push $0
    push $27
    jmp isr_common_stub

# 28: Reserved
isr28:
    cli
    push $0
    push $28
    jmp isr_common_stub

# 29: Reserved
isr29:
    cli
    push $0
    push $29
    jmp isr_common_stub

# 30: Reserved
isr30:
    cli
    push $0
    push $30
    jmp isr_common_stub

# 31: Reserved
isr31:
    cli
    push $0
    push $31
    jmp isr_common_stub

# IRQs handlers.
irq0:
    cli
    push $0
    push $32
    jmp irq_common_stub

irq1:
    cli
    push $1
    push $33
    jmp irq_common_stub

irq2:
    cli
    push $2
    push $34
    jmp irq_common_stub

irq3:
    cli
    push $3
    push $35
    jmp irq_common_stub

irq4:
    cli
    push $4
    push $36
    jmp irq_common_stub

irq5:
    cli
    push $5
    push $37
    jmp irq_common_stub

irq6:
    cli
    push $6
    push $38
    jmp irq_common_stub

irq7:
    cli
    push $7
    push $39
    jmp irq_common_stub

irq8:
    cli
    push $8
    push $40
    jmp irq_common_stub

irq9:
    cli
    push $9
    push $41
    jmp irq_common_stub

irq10:
    cli
    push $10
    push $42
    jmp irq_common_stub

irq11:
    cli
    push $11
    push $43
    jmp irq_common_stub

irq12:
    cli
    push $12
    push $44
    jmp irq_common_stub

irq13:
    cli
    push $13
    push $45
    jmp irq_common_stub

irq14:
    cli
    push $14
    push $46
    jmp irq_common_stub

irq15:
    cli
    push $15
    push $47
    jmp irq_common_stub
