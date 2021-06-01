#include <Kernel/PCI/pci.h>

void pci_init(void)
{
    puts("Reading all PCI vendors...\n");
    for (uint8_t bus = 0; bus < 0xFF; bus++)
    {
        uint16_t vendor = pci_check_vendor(bus, 0);
        printf("PCI vendor on bus %d: 0x%H.\n", bus, vendor);

        for (uint32_t d = 0; d < 0x0FFFFFFF; d++)
            __asm__ __volatile__ ("nop");
    }
}

uint16_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t lbus = (uint32_t) bus;
    uint32_t lslot = (uint32_t) slot;
    uint32_t lfunc = (uint32_t) func;

    uint32_t address = (uint32_t) ((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t) 0x80000000));
    io_outl(CONFIG_ADDRESS, address);

    uint16_t ret = (uint16_t) ((io_inl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
    return ret;
}

uint16_t pci_check_vendor(uint8_t bus, uint8_t slot)
{
    uint16_t vendor = pci_read_config(bus, slot, 0, 0);
    return vendor;
}