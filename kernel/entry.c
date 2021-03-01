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

#include <sys/syscall.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int k_entry(multiboot_info_t* mbt, uint32_t stack)
{
    gdt_init();
    isr_init();
    tty_init();
    logger_init();

    if (mbt->mods_count > 0)
        abort();

    // TODO: install initrd filesystem before memory managment.

    kmem_init(mbt);
    page_init();
    task_init(stack);

    klog(INFO, "Initializing hardware !");
    timer_init();
    keyboard_init();
    klog(INFO, "Done.");

    printf("Welcome to TheOS !\n");

    klog(INFO, "Entering in user-mode...");
    switch_to_user_mode();

    syscall(0, (uint32_t) &"In user-mode with syscall !!!\n", 0, 0, 0, 0);

    for (;;);
}