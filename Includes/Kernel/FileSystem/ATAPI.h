#ifndef _KERNEL_ATAPI_H
#define _KERNEL_ATAPI_H

#include <Kernel/CPU/io.h>

#include <stdint.h>
#include <stdio.h>

#define PRIMARY_DEVICE_CONTROL_REG      0x3F6
#define SECONDARY_DEVICE_CONTROL_REG    0x376

#define INTERRUPT_DISABLE   (1 << 1)
#define SECTOR_COUNT_RESET  (1 << 2)

#define SECTOR_COUNT    0x1F2
#define LBA_LOW         0x1F3
#define LBA_MID         0x1F4
#define LBA_HIGH        0x1F5

#define NPACK_DEVICE    0xEC
#define PACK_DEVICE     0xA1

void ATAPI_init(void);

#endif