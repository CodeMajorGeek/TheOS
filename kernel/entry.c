
#include <kernel/logger.h>

void k_entry(void)
{
    logger_init();

    *((int *) 0xB8000) = 0x07690748;

    for(;;);
}