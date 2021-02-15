
#include <kernel/logger.h>

void k_entry(void)
{
    logger_init();
    klog(INFO, "Booted successfully !");

    

    while (true);
}