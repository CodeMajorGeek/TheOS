#include <kernel/gdt.h>
#include <kernel/isr.h>
#include <kernel/logger.h>
#include <kernel/tty.h>
#include <kernel/keyboard.h>
#include <kernel/tss.h>

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

    printf("Welcome to TheOS !\n");

    tss_switch_usermode();

    while (true)
        __asm__("hlt");
}