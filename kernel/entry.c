
#include <kernel/logger.h>
#include <kernel/tty.h>

#include <stdbool.h>

void k_entry(void)
{
    logger_init();

    klog(INFO, "Initializing hardware !");
    tty_init();
    klog(INFO, "Hardware initialized !");

    tty_puts("Je suis sur le tty !");

    while (true);
}