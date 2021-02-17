#include <kernel/gdt.h>
#include <kernel/isr.h>
#include <kernel/logger.h>
#include <kernel/tty.h>
#include <kernel/io.h>

#include <stdbool.h>
#include <stdio.h>

void k_entry(void)
{
    gdt_init();
    isr_init();

    logger_init();
    klog(DEBUG, "Initializing hardware !\r\n");
    tty_init();
    klog(INFO, "Hardware initialized !\r\n");

    puts("Je suis un test !");

    io_outb(0x21, 0xFD);

    while (true)
        __asm__("hlt");
}