
#include <kernel/logger.h>
#include <kernel/tty.h>

#include <stdbool.h>
#include <stdlib.h>

void k_entry(void)
{
    logger_init();

    klog(INFO, "Initializing hardware !\r\n");
    tty_init();
    klog(INFO, "Hardware initialized !\r\n");

    tty_puts("Je suis sur le tty !");

    abort();

    while (true);
}