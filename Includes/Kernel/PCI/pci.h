#ifndef _KERNEL_PCI_H
#define _KERNEL_PCI_H

#include <Kernel/CPU/io.h>

#include <stdint.h>
#include <stdio.h>

#define CONFIG_ADDRESS  0xCF8
#define CONFIG_DATA     0xCFC

typedef struct
{
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t BIST;
    uint32_t base_address0;
    uint32_t base_address1;
    uint32_t base_address2;
    uint32_t base_address3;
    uint32_t base_address4;
    uint32_t base_address5;
    uint32_t bardbus_CIS_pointer;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_ROM_base_address;
    uint8_t capailities_pointer;
    uint8_t reserved0;
    uint16_t reserved1;
    uint32_t reserved2;
    uint8_t interrupt_line;
    uint8_t interrupt_PIN;
    uint8_t min_grant;
    uint8_t max_latency;
} PCI_header_t;

void pci_init(void);

uint16_t pci_read_config(uint8_t, uint8_t, uint8_t, uint8_t);
uint16_t pci_check_vendor(uint8_t, uint8_t);

#endif