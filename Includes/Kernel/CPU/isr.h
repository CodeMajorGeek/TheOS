#ifndef _KERNEL_ISR_H
#define _KERNEL_ISR_H

#include <Kernel/CPU/io.h>
#include <Kernel/CPU/idt.h>
#include <Kernel/Memory/page.h>
#include <Kernel/CPU/system.h>
#include <Kernel/Utils/logger.h>
#include <Kernel/Process/syscall.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PIC1        0x20
#define PIC2        0xA0
#define PIC1_CTRL   PIC1
#define PIC2_CTRL   PIC2
#define PIC1_DATA   (PIC1 + 1)
#define PIC2_DATA   (PIC2 + 2)
#define PIC_EOI     0x20
#define ICW1        0x11
#define ICW4        0x01

#define MAX_KNOWN_EXCEPTIONS 19

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

/* ISRs reserved for CPU exceptions. */
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

/* ISRs Reserved for the kernel. */
extern void isr128(void);

/* IRQ definitions. */
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

void isr_handler(registers_t);
void irq_handler(registers_t);

void isr_init(void);
void isr_register_interrupt_handler(int, isr_t);

#endif