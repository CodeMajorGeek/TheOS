#include <Kernel/CPU/timer.h>

uint32_t ticks = 0;
uint16_t task_ticks = 0;

static void timer_callback(registers_t* regs)
{
    ticks++;
    if (task_ticks++ >= 100)
    {
        task_switch();
        task_ticks = 0;
    }
}

void timer_init(void)
{
    timer_phase(1); // Fire every 1ms.
    isr_register_interrupt_handler(IRQ0, timer_callback);
}

void timer_phase(uint16_t frequency)
{
    uint16_t divisor = BASE_FREQUENCY / frequency;
    io_outb(0x43, 0x36);
    io_outb(0x40, divisor & 0xFF);
    io_outb(0x40, divisor >> 8);
}

void sleep(uint32_t ms)
{
    ticks = 0;
    while (ticks <= ms)
        __asm__("nop");
}