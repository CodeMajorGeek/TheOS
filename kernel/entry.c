#include <boot/multiboot.h>

#include <kernel/io.h>
#include <kernel/gdt.h>
#include <kernel/isr.h>
#include <kernel/tss.h>
#include <kernel/tty.h>
#include <kernel/kmem.h>
#include <kernel/page.h>
#include <kernel/task.h>
#include <kernel/timer.h>
#include <kernel/logger.h>
#include <kernel/keyboard.h>

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
    task_init(stack);

    klog(INFO, "Initializing hardware !");
    tty_init();
    timer_init();
    keyboard_init();
    klog(INFO, "Done.");

    klog(INFO, "Entering in user-mode...");
    switch_to_user_mode();
    klog(INFO, "Done.");

    for (;;); // Endless loop.
}