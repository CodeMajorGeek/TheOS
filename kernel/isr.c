#include <kernel/isr.h>

isr_t interrupt_handlers[MAX_ENTRIES];

const char* exception_messages[MAX_KNOWN_EXCEPTIONS] = 
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check"
};

void isr_init(void)
{
    /* CPU reserved ISRs. */
    idt_set_gate(0, (uint32_t) isr0);
    idt_set_gate(1, (uint32_t) isr1);
    idt_set_gate(2, (uint32_t) isr2);
    idt_set_gate(3, (uint32_t) isr3);
    idt_set_gate(4, (uint32_t) isr4);
    idt_set_gate(5, (uint32_t) isr5);
    idt_set_gate(6, (uint32_t) isr6);
    idt_set_gate(7, (uint32_t) isr7);
    idt_set_gate(8, (uint32_t) isr8);
    idt_set_gate(9, (uint32_t) isr9);
    idt_set_gate(10, (uint32_t) isr10);
    idt_set_gate(11, (uint32_t) isr11);
    idt_set_gate(12, (uint32_t) isr12);
    idt_set_gate(13, (uint32_t) isr13);
    idt_set_gate(14, (uint32_t) isr14);
    idt_set_gate(15, (uint32_t) isr15);
    idt_set_gate(16, (uint32_t) isr16);
    idt_set_gate(17, (uint32_t) isr17);
    idt_set_gate(18, (uint32_t) isr18);
    idt_set_gate(19, (uint32_t) isr19);
    idt_set_gate(20, (uint32_t) isr20);
    idt_set_gate(21, (uint32_t) isr21);
    idt_set_gate(22, (uint32_t) isr22);
    idt_set_gate(23, (uint32_t) isr23);
    idt_set_gate(24, (uint32_t) isr24);
    idt_set_gate(25, (uint32_t) isr25);
    idt_set_gate(26, (uint32_t) isr26);
    idt_set_gate(27, (uint32_t) isr27);
    idt_set_gate(28, (uint32_t) isr28);
    idt_set_gate(29, (uint32_t) isr29);
    idt_set_gate(30, (uint32_t) isr30);
    idt_set_gate(31, (uint32_t) isr31);

    /* Kernel reserved ISRs. */
    idt_set_gate(128, (uint32_t) isr128);

    /* ICW1: Begin initialization. */
    io_outb(PIC1_CTRL, ICW1);
    io_outb(PIC2_CTRL, ICW1);

    /* ICW2: Remap offset address of the idt_table. */
    io_outb(PIC1_DATA, 0x20);
    io_outb(PIC2_DATA, 0x28);

    /* ICW3: Setup cascading. */
    io_outb(PIC1_DATA, 0x4);
    io_outb(PIC2_DATA, 0x2);

    /* ICW4: Environment info. */
    io_outb(PIC1_DATA, 0x01);
    io_outb(PIC2_DATA, 0x01);

    /* Clear mask register. */
    io_outb(PIC1_DATA, 0x00);
    io_outb(PIC2_DATA, 0x00);

    /* Setup the IRQs. */
    idt_set_gate(32, (uint32_t) irq0);
    idt_set_gate(33, (uint32_t) irq1);
    idt_set_gate(34, (uint32_t) irq2);
    idt_set_gate(35, (uint32_t) irq3);
    idt_set_gate(36, (uint32_t) irq4);
    idt_set_gate(37, (uint32_t) irq5);
    idt_set_gate(38, (uint32_t) irq6);
    idt_set_gate(39, (uint32_t) irq7);
    idt_set_gate(40, (uint32_t) irq8);
    idt_set_gate(41, (uint32_t) irq9);
    idt_set_gate(42, (uint32_t) irq10);
    idt_set_gate(43, (uint32_t) irq11);
    idt_set_gate(44, (uint32_t) irq12);
    idt_set_gate(45, (uint32_t) irq13);
    idt_set_gate(46, (uint32_t) irq14);
    idt_set_gate(47, (uint32_t) irq15);

    idt_set();
}

void isr_register_interrupt_handler(int index, isr_t handler)
{
    if (index < MAX_ENTRIES)
        interrupt_handlers[index] = handler;
}

void isr_handler(registers_t r)
{
    if (r.int_no == 14)
        page_fault_handler(&r);
    else if (r.int_no < MAX_KNOWN_EXCEPTIONS)
    {
        const char* exception_message = exception_messages[r.int_no];
        size_t message_len = 18 + strlen(exception_message);
        char buf[message_len];
        memset(&buf, '\0', message_len);
        sprintf(buf, "Received ISR:\r\n\t%s.", exception_message);
        klog(FATAL, buf);
    }
    else if (r.int_no == 0x80)
    {
        klog(DEBUG, "Syscall interrupt !");
        syscall_handler(&r);
        return;
    }
    else
        klog(FATAL, "Received ISR:\r\n\tReserved.");
    abort();
}

void irq_handler(registers_t r)
{   
    /* Send EOI to the PICs. */
    if (r.int_no >= 40)
        io_outb(PIC2, PIC_EOI); // Slave.
    io_outb(PIC1, PIC_EOI); // Master.

    isr_t handler = interrupt_handlers[r.int_no];
    if (handler != 0)
        handler(&r);
}