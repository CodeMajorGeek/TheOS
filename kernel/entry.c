
#include <kernel/logger.h>
#include <kernel/tty.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

void k_entry(void)
{
    logger_init();

    klog(DEBUG, "Initializing hardware !\r\n");
    tty_init();
    klog(INFO, "Hardware initialized !\r\n");

    printf("salut %s !", "toi");

    abort();

    while (true);
}