#include <Kernel/Devices/keyboard.h>

// TODO: make keycodes adaptable by using a config file stored in initrd...
const char keycodes[27] = {  };

char last_scancode = NULL;

static void keyboard_callback(registers_t* regs)
{
    last_scancode = io_inb(PORT_SCANCODE);
}

void keyboard_init(void)
{
    isr_register_interrupt_handler(IRQ1, keyboard_callback);
}

uint8_t keyboard_wait_scancode(void)
{
    while (!last_scancode)
        __asm__("nop");
    char ret = (char) last_scancode;
    last_scancode = NULL;
    return ret;
}

char keyboard_wait_ASCII(void)
{

}