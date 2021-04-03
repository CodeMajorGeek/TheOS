#include <Kernel/timer.h>

uint32_t ticks = 0;

static void timer_callback(registers_t* regs)
{
    ticks++;
    task_switch();
}

void timer_init(void)
{
    timer_phase(100); // Fire every 100ms.
    isr_register_interrupt_handler(IRQ0, timer_callback);
}

void timer_phase(uint16_t frequency)
{
    uint16_t divisor = BASE_FREQUENCY / frequency;
    io_outb(0x43, 0x36);
    io_outb(0x40, divisor & 0xFF);
    io_outb(0x40, divisor >> 8);
}