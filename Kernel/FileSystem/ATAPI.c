#include <Kernel/FileSystem/ATAPI.h>

uint8_t id_device = 0x00;

void ATAPI_init(void)
{

    io_outb(PRIMARY_DEVICE_CONTROL_REG, SECTOR_COUNT_RESET);
    io_outb(PRIMARY_DEVICE_CONTROL_REG, INTERRUPT_DISABLE);

    uint8_t sector_count = io_inb(SECTOR_COUNT);
    uint8_t LBA_low = io_inb(LBA_LOW);
    uint8_t LBA_mid = io_inb(LBA_MID);
    uint8_t LBA_high = io_inb(LBA_HIGH);

    printf("Sector count: 0x%H, LBA: 0x%H 0x%H 0x%H.\n", sector_count, LBA_high, LBA_mid, LBA_low);

    if (sector_count == 0x01 && LBA_low ==0x01 && LBA_mid == 0x00 && LBA_high == 0x00)
    {
        puts("The ATA disk is a non-packet device !");
        id_device = NPACK_DEVICE;
    } else if (sector_count == 0x01 && LBA_low == 0x01 && LBA_mid == 0x14 && LBA_high == 0xEB)
    {
        puts("The ATA disk is a packet device !");
        id_device = PACK_DEVICE;
    }
}