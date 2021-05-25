#include <Boot/multiboot.h>

#include <Kernel/CPU/io.h>
#include <Kernel/CPU/gdt.h>
#include <Kernel/CPU/isr.h>
#include <Kernel/CPU/tss.h>
#include <Kernel/Devices/tty.h>
#include <Kernel/FileSystem/vfs.h>
#include <Kernel/FileSystem/ATAPI.h>
#include <Kernel/Memory/kmem.h>
#include <Kernel/Memory/page.h>
#include <Kernel/Process/task.h>
#include <Kernel/CPU/timer.h>
#include <Kernel/Utils/logger.h>
#include <Kernel/FileSystem/initrd.h>
#include <Kernel/Devices/keyboard.h>

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

    klog(INFO, "Initializing initrd...");
    uint32_t initrd_location = *((uint32_t*) mbi->mods_addr);
    uint32_t initrd_end = *(uint32_t*)(mbi->mods_addr + 4);
    kmem_addr = initrd_end; // Put memory managment after initrtd to prevent overwrite it.

    kmem_init(mbi);
    page_init();
    task_init(stack);

    fs_root = initrd_init(initrd_location);
    klog(INFO, "Done.");
    
    puts("================================================================================");
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
            uint8_t* offset;
            uint32_t s = read_fs(fsnode, 0, fsnode->length, offset);
            printf("Loaded at offset: 0x%H\n", offset);
        }
    }

    puts("\n================================================================================");   

    klog(INFO, "Initializing hardware...");
    timer_init();
    keyboard_init();
    klog(INFO, "Done.");

    klog(INFO, "Initializing storage...");
    ATAPI_init();
    klog(INFO, "Done.");

    puts("Welcome to TheOS !\n");

    klog(INFO, "Entering in user-mode...");
    switch_to_user_mode();
    
    $sys$puts("In user-mode with syscalls !!!\n");



    for (;;);
}