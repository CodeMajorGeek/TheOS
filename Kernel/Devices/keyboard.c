#include <Kernel/Devices/keyboard.h>

static void keyboard_callback(registers_t* regs)
{
    printf("Keyboard interrupt IRQ1, scancode: %H\n", io_inb(0x60));
}

void keyboard_init(void)
{
    isr_register_interrupt_handler(IRQ1, keyboard_callback);
}