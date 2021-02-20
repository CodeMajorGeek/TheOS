#include <boot/multiboot.h>

#include <kernel/gdt.h>
#include <kernel/isr.h>
#include <kernel/logger.h>
#include <kernel/kmem.h>
#include <kernel/page.h>
#include <kernel/tty.h>
#include <kernel/keyboard.h>
#include <kernel/tss.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void k_entry(multiboot_info_t* mbt, uint32_t stack)
{
    gdt_init();
    isr_init();
    logger_init();

    if (mbt->mods_count > 0)
        abort();

    // TODO: install initrd filesystem before memory managment.

    kmem_init(mbt);
    page_init();

    
    klog(INFO, "Initializing hardware !");
    tty_init();
    keyboard_init();
    klog(INFO, "Done.");

    /*klog(INFO, "Entering un user-mode...");
    tss_switch();*/

    printf("Welcome to TheOS !\n");

    while (true)
        __asm__("hlt");
}