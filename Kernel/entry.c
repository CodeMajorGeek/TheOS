#include <boot/multiboot.h>

#include <kernel/io.h>
#include <kernel/gdt.h>
#include <kernel/isr.h>
#include <kernel/tss.h>
#include <kernel/tty.h>
#include <kernel/vfs.h>
#include <kernel/kmem.h>
#include <kernel/page.h>
#include <kernel/task.h>
#include <kernel/timer.h>
#include <kernel/logger.h>
#include <kernel/initrd.h>
#include <kernel/keyboard.h>

#include <sys/syscall.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

int k_entry(uint32_t magic, uint32_t addr, uint32_t stack)
{
    tty_init();
    logger_init();
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        klog(FATAL, "Invalid multiboot magic number !");
        abort();
    }
    gdt_init();
    isr_init();
    
    multiboot_info_t* mbi = (multiboot_info_t*) addr;

    if (mbi->mods_count <= 0)
    {
        klog(FATAL, "Multiboot can't find initrd module !");
        abort();
    }

    kmem_init(mbi);

    klog(INFO, "Initializing initrd...");
    uint32_t initrd_location = *((uint32_t*) mbi->mods_addr);
    fs_root = initrd_init(initrd_location);
    klog(INFO, "Done.");

    
    puts("================================================================================\n");
    puts("Before going to user-mode, let's test VFS:\n");

    dirent_t* node = 0;
    int i = 0;
    while ((node = readdir_fs(fs_root, i++)) != 0)
    {
        printf("Found file \"%s\":\n", node->name);

        fs_node_t* fsnode = finddir_fs(fs_root, node->name);
        if ((fsnode->flags & 0x7) == FS_DIRECOTRY)
            puts("\t(directory).\n");
        else
        {
            char buf[fsnode->length];
            uint32_t s = read_fs(fsnode, 0, fsnode->length, buf);
            printf("\tContents of size %d-%d: \"%s\".\n", s, fsnode->length, buf);
        }
    }

    puts("\n================================================================================\n");

    page_init();

    task_init(stack);   

    klog(INFO, "Initializing hardware...");
    timer_init();
    keyboard_init();
    klog(INFO, "Done.");

    puts("Welcome to TheOS !\n");

    klog(INFO, "Entering in user-mode...");
    switch_to_user_mode();
    
    syscall(0, (uint32_t) &"In user-mode with syscalls !!!\n", 0, 0, 0, 0);

    for (;;);
}