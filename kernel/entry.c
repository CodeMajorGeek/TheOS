#include <kernel/gdt.h>
#include <kernel/isr.h>
#include <kernel/logger.h>
#include <kernel/tty.h>
#include <kernel/keyboard.h>

#include <stdbool.h>
#include <stdio.h>

void k_entry(void)
{
    gdt_init();
    isr_init();

    logger_init();
    klog(INFO, "Initializing hardware !\r\n");
    tty_init();
    keyboard_init();
    klog(INFO, "Hardware initialized !\r\n");
    
    __asm__ __volatile__("int $13");

    while (true)
        __asm__("hlt");
}