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
    klog(INFO, "Initializing hardware !");
    tty_init();
    keyboard_init();
    klog(INFO, "Hardware initialized !");
    
    __asm__ __volatile__("int $13");

    char buf[512];
    sprintf(buf, "Je suis un test: %d", 1024);

    puts(buf);

    while (true)
        __asm__("hlt");
}