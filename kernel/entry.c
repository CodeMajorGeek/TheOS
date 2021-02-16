
#include <kernel/logger.h>
#include <kernel/tty.h>
#include <kernel/memory.h>

#include <stdbool.h>
#include <stdio.h>

void k_entry(void)
{
    logger_init();

    klog(DEBUG, "Initializing hardware !\r\n");
    tty_init();
    klog(INFO, "Hardware initialized !\r\n");

    while (true);
}