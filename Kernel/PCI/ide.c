#include <Kernel/PCI/ide.h>

ide_channel_reg_t channels[2];
ide_device_t ide_devices[4];

uint8_t ide_buf[2048] = { 0 };
static uint8_t ide_irq_invoked = 0;
static uint8_t atapi_packet[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static void ide_irq(registers_t* regs)
{
    ide_irq_invoked = 1;
}

void ide_write(uint8_t channel, uint8_t reg, uint8_t data)
{
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);

    if (reg < 0x08)
        io_outb(channels[channel].base  + reg - 0x00, data);
    else if (reg < 0x0C)
        io_outb(channels[channel].base  + reg - 0x06, data);
    else if (reg < 0x0E)
        io_outb(channels[channel].ctrl  + reg - 0x0A, data);
    else if (reg < 0x16)
        io_outb(channels[channel].bmide + reg - 0x0E, data);

    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

uint8_t ide_read(uint8_t channel, uint8_t reg)
{
    uint8_t result;

    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    
    if (reg < 0x08)
        result = io_inb(channels[channel].base + reg - 0x00);
    else if (reg < 0x0C)
        result = io_inb(channels[channel].base  + reg - 0x06);
    else if (reg < 0x0E)
        result = io_inb(channels[channel].ctrl  + reg - 0x0A);
    else if (reg < 0x16)
        result = io_inb(channels[channel].bmide + reg - 0x0E);

    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);

    return result;
}

void ide_read_buffer(uint8_t channel, uint8_t reg, uint32_t* buffer, uint32_t quads)
{
    /* WARNING: This code contains a serious bug. The inline assembly trashes ES and
    *           ESP for all of the code the compiler generates between the inline
    *           assembly blocks. */
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    
    __asm__("pushw %es; movw %ds, %ax; movw %ax, %es");

    if (reg < 0x08)
        io_insl(channels[channel].base  + reg - 0x00, buffer, quads);
    else if (reg < 0x0C)
        io_insl(channels[channel].base  + reg - 0x06, buffer, quads);
    else if (reg < 0x0E)
        io_insl(channels[channel].ctrl  + reg - 0x0A, buffer, quads);
    else if (reg < 0x16)
        io_insl(channels[channel].bmide + reg - 0x0E, buffer, quads);
    
    __asm__("popw %es;");

    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

uint8_t ide_polling(uint8_t channel, uint32_t advanced_check)
{
    int i = 0;

    /* Wait 400 ns for BSY to be set. */
    for(i = 0; i < 4; i++)
        ide_read(channel, ATA_REG_ALTSTATUS); // Reading the alternate status port wastes 100 ns, loop 4 times.

    /* Wait for BSY to be cleared. */
    while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY)
        __asm__("nop"); // Wait for BSY to be zero.
    
    if (advanced_check)
    {
        uint8_t state = ide_read(channel, ATA_REG_STATUS); // Read status register.
    
        /* Catch errors. */
        if (state & ATA_SR_ERR)
            return 2; // Error.
    
        /* Catch device faults. */
        if (state & ATA_SR_DF)
            return 1; // Device fault.
    
        /* Check DRQ:
         * BSY = 0; DF = 0; ERR = 0 so we check for DRQ now. */
        if ((state & ATA_SR_DRQ) == 0)
            return 3; // DRQ have to be set.
    
    }
    return 0; // Everything is fine.
}

uint8_t ide_print_error(uint32_t drive, uint8_t err)
{
    if (err == 0)
        return err;
    
    puts("IDE:");
    if (err == 1) 
    {
        printf("- Device Fault\n\t");
        err = 19;
    }
    else if (err == 2) {
        uint8_t st = ide_read(ide_devices[drive].channel, ATA_REG_ERROR);
        if (st & ATA_ER_AMNF) 
        {
            printf("- No Address Mark Found\n\t");
            err = 7;
        }
        if (st & ATA_ER_TK0NF) 
        {
            printf("- No Media or Media Error\n\t");
            err = 3;
        }
        if (st & ATA_ER_ABRT) 
        {
            printf("- Command Aborted\n\t");
            err = 20;
        }
        if (st & ATA_ER_MCR) 
        {
            printf("- No Media or Media Error\n\t");
            err = 3;
        }
        if (st & ATA_ER_IDNF) 
        {
            printf("- ID mark not Found\n\t");
            err = 21;
        }
        if (st & ATA_ER_MC)
        {
            printf("- No Media or Media Error\n\t");
            err = 3;
        }
        if (st & ATA_ER_UNC) 
        {
            printf("- Uncorrectable Data Error\n\t");
            err = 22;
        }
        if (st & ATA_ER_BBK) 
        {
            printf("- Bad Sectors\n\t");
            err = 13;
        }
    } 
    else  if (err == 3) 
    {
        printf("- Reads Nothing\n\t");
        err = 23;
    } 
    else  if (err == 4)
    {
        printf("- Write Protected\n\t");
        err = 8;
    }
    printf("- [%s %s] %s\n",
        (const char *[]){ "Primary", "Secondary" }[ide_devices[drive].channel], // Use the channel as an index into the array.
        (const char *[]){ "Master", "Slave" }[ide_devices[drive].drive], // Same as above but using drive instead.
        ide_devices[drive].model);
    
    return err;
}

void ide_initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4) 
{
    int count = 0;

    /* Detect I/O ports which interface IDE Controller. */
    channels[ATA_PRIMARY  ].base  = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
    channels[ATA_PRIMARY  ].ctrl  = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
    channels[ATA_SECONDARY].base  = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
    channels[ATA_SECONDARY].ctrl  = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
    channels[ATA_PRIMARY  ].bmide = (BAR4 & 0xFFFFFFFC) + 0; // Bus Master IDE.
    channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE.

    /* Disable IRQs: */
    ide_write(ATA_PRIMARY  , ATA_REG_CONTROL, 2);
    ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
        {
            uint8_t err = 0, type = IDE_ATA, status;
            ide_devices[count].reserved = 0; // Assuming that no drive here.
    
            /* Select the drive. */
            ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select the drive.
            sleep(1); // Wait 1ms for drive select to work.

            /* Send ATA Identify Command. */
            ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
            sleep(1);

            /* Polling. */
            if (ide_read(i, ATA_REG_STATUS) == 0) 
                continue; // If Status = 0 then no device.
    
            while(TRUE)
            {
                status = ide_read(i, ATA_REG_STATUS);
                if ((status & ATA_SR_ERR)) // If Err then device is not an ATA one.
                {
                    err = 1;
                    break;
                } 
                if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ))
                    break; // Everything is right.
            }

            /* Probe for ATAPI devices (TODO implement r/w to ATAPI). */
            if (err != 0) {
                uint8_t cl = ide_read(i, ATA_REG_LBA1);
                uint8_t ch = ide_read(i, ATA_REG_LBA2);
    
                if (cl == 0x14 && ch ==0xEB)
                    type = IDE_ATAPI;
                else if (cl == 0x69 && ch == 0x96)
                    type = IDE_ATAPI;
                else
                    continue; // Unknown type (may not be a device).
    
                ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
                sleep(1);
            }

            /* Read identification space of the device. */
            ide_read_buffer(i, ATA_REG_DATA, (uint32_t*) ide_buf, 128);

            // (VI) Read Device Parameters:
            ide_devices[count].reserved     = 1;
            ide_devices[count].type         = type;
            ide_devices[count].channel      = i;
            ide_devices[count].drive        = j;
            ide_devices[count].signature    = *((uint16_t*)(ide_buf + ATA_IDENT_DEVICETYPE));
            ide_devices[count].capabilities = *((uint16_t*)(ide_buf + ATA_IDENT_CAPABILITIES));
            ide_devices[count].commandSets  = *((uint32_t*)(ide_buf + ATA_IDENT_COMMANDSETS));
    
            /* Get the size. */
            if (ide_devices[count].commandSets & (1 << 26))
                ide_devices[count].size   = *((uint32_t*)(ide_buf + ATA_IDENT_MAX_LBA_EXT)); // Device uses 48-Bit addressing.
            else
                
                ide_devices[count].size   = *((uint32_t*)(ide_buf + ATA_IDENT_MAX_LBA)); // Device uses CHS or 28-bit addressing.

            /* String indicates model of device. */
            for(int k = 0; k < 40; k += 2)
            {
                ide_devices[count].model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
                ide_devices[count].model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];
            }
            ide_devices[count].model[40] = '\0'; // End of string.
            count++;
        }

    isr_register_interrupt_handler(IRQ14, ide_irq); // Register the ATAPI irq.

    /*Print avaliable ATA IDE. */
    for (int i = 0; i < 4; i++)
        if (ide_devices[i].reserved == 1)
        {
            printf("Found %s Drive %dGB - Model: \"%s\"\n",
            (const char* []){"ATA", "ATAPI"}[ide_devices[i].type],  // The type.
            ide_devices[i].size / 1024 / 1024 / 2,                  // The size (in GB);
            ide_devices[i].model);
        }
}

uint8_t ide_ata_access(uint8_t direction, uint8_t drive, uint32_t lba, uint8_t numsects, uint16_t selector, uint32_t edi)
{
    uint8_t     lba_mode /* 0: CHS, 1:LBA28, 2: LBA48 */, dma /* 0: No DMA, 1: DMA */, cmd;
    uint8_t     lba_io[6];
    uint32_t    channel     = ide_devices[drive].channel; // Read the channel.
    uint32_t    slavebit    = ide_devices[drive].drive; // Read the drive [Master/Slave].
    uint32_t    bus         = channels[channel].base; // Bus base, like 0x1F0 which is also data port.
    uint32_t    words       = 256; // Almost every ATA drive has a sector-size of 512-byte.
    uint16_t    cyl;
    uint8_t     head, sect, err;

    ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = (ide_irq_invoked = 0x0) + 0x02);

    /* Select one from LBA28, LBA48 or CHS. */
    if (lba >= 0x10000000)
    { 
        /* LBA48: */
        lba_mode  = 2;
        lba_io[0] = (lba & 0x000000FF) >> 0;
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = (lba & 0xFF000000) >> 24;
        lba_io[4] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
        lba_io[5] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
        head      = 0; // Lower 4-bits of HDDEVSEL are not used here.
    } 
    else if (ide_devices[drive].capabilities & 0x200) // Drive supports LBA?
    { 
        /* LBA28: */
        lba_mode  = 1;
        lba_io[0] = (lba & 0x00000FF) >> 0;
        lba_io[1] = (lba & 0x000FF00) >> 8;
        lba_io[2] = (lba & 0x0FF0000) >> 16;
        /* These 3 registers are not used here. */
        lba_io[3] = 0; 
        lba_io[4] = 0;
        lba_io[5] = 0;
        head      = (lba & 0xF000000) >> 24;
    } 
    else
    {
        /* CHS: */
        lba_mode  = 0;
        sect      = (lba % 63) + 1;
        cyl       = (lba + 1  - sect) / (16 * 63);
        lba_io[0] = sect;
        lba_io[1] = (cyl >> 0) & 0xFF;
        lba_io[2] = (cyl >> 8) & 0xFF;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head      = (lba + 1  - sect) % (16 * 63) / (63); // Head number is written to HDDEVSEL lower 4-bits.
    }

    /* See if drive supports DMA or not. */
    dma = 0; // We don't support DMA (TODO: implement DMA).

    /* Wait if the drive is busy. */
    while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY)
        __asm__("nop"); // Wait if busy.

     /* Select the drive from the controller. */
    if (lba_mode == 0)
        ide_write(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head); // Drive & CHS.
    else
        ide_write(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head); // Drive & LBA

       /* Write parameters. */
    if (lba_mode == 2)
    {
        ide_write(channel, ATA_REG_SECCOUNT1,   0);
        ide_write(channel, ATA_REG_LBA3,   lba_io[3]);
        ide_write(channel, ATA_REG_LBA4,   lba_io[4]);
        ide_write(channel, ATA_REG_LBA5,   lba_io[5]);
    }
    ide_write(channel, ATA_REG_SECCOUNT0,   numsects);
    ide_write(channel, ATA_REG_LBA0,   lba_io[0]);
    ide_write(channel, ATA_REG_LBA1,   lba_io[1]);
    ide_write(channel, ATA_REG_LBA2,   lba_io[2]);

    /* Select the command and send it.
     * If ( DMA & LBA48)   DO_DMA_EXT;
     * If ( DMA & LBA28)   DO_DMA_LBA;
     * If ( DMA & LBA28)   DO_DMA_CHS;
     * If (!DMA & LBA48)   DO_PIO_EXT;
     * If (!DMA & LBA28)   DO_PIO_LBA;
     * If (!DMA & !LBA#)   DO_PIO_CHS; */

    if (lba_mode == 0 && dma == 0 && direction == 0)
        cmd = ATA_CMD_READ_PIO;
    if (lba_mode == 1 && dma == 0 && direction == 0)
        cmd = ATA_CMD_READ_PIO;   
    if (lba_mode == 2 && dma == 0 && direction == 0)
        cmd = ATA_CMD_READ_PIO_EXT;   
    if (lba_mode == 0 && dma == 1 && direction == 0)
        cmd = ATA_CMD_READ_DMA;
    if (lba_mode == 1 && dma == 1 && direction == 0)
        cmd = ATA_CMD_READ_DMA;
    if (lba_mode == 2 && dma == 1 && direction == 0)
        cmd = ATA_CMD_READ_DMA_EXT;
    if (lba_mode == 0 && dma == 0 && direction == 1)
        cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 1 && dma == 0 && direction == 1)
        cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 2 && dma == 0 && direction == 1) 
        cmd = ATA_CMD_WRITE_PIO_EXT;
    if (lba_mode == 0 && dma == 1 && direction == 1) 
        cmd = ATA_CMD_WRITE_DMA;
    if (lba_mode == 1 && dma == 1 && direction == 1) 
        cmd = ATA_CMD_WRITE_DMA;
    if (lba_mode == 2 && dma == 1 && direction == 1) 
        cmd = ATA_CMD_WRITE_DMA_EXT;
    ide_write(channel, ATA_REG_COMMAND, cmd);  // Send the command.

    if (dma)
    {
        if (direction == 0);
            // DMA read.
        else;
            // DMA write.
    }
    else
    {
        if (direction == 0) // PIO Read.
        { 
            for (uint8_t i = 0; i < numsects; i++)
            {
                if (err = ide_polling(channel, 1))
                    return err; // Polling, set error and exit if there is.
                io_insw(bus, edi, words);
                edi += (words * 2);
            }
        } 
        else // PIO write.
        {
            for (uint8_t i = 0; i < numsects; i++)
            {
                printf("", i);

                ide_polling(channel, 0); // Polling.
                io_outsw(bus, edi, words);
                edi += (words * 2);
            }
            ide_write(channel, ATA_REG_COMMAND, (char[]) { ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH_EXT }[lba_mode]);
            ide_polling(channel, 0); // Polling.
        }
    }
    
    return err; // Done !
}

void ide_wait_irq(void)
{
   while (!ide_irq_invoked)
      __asm__("nop");
   ide_irq_invoked = 0;
}

uint8_t ide_atapi_read(uint8_t drive, uint32_t lba, uint8_t numsects, uint16_t selector, uint32_t edi)
{
    uint32_t   channel  = ide_devices[drive].channel;
    uint32_t   slavebit = ide_devices[drive].drive;
    uint32_t   bus      = channels[channel].base;
    uint32_t   words    = 1024; // Sector size. ATAPI drives have a sector size of 2048 bytes.
    uint8_t    err;

    ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = ide_irq_invoked = 0x0); // Enable IRQs.

    /* Setup SCSI packet. */
    atapi_packet[ 0] = ATAPI_CMD_READ;
    atapi_packet[ 1] = 0x0;
    atapi_packet[ 2] = (lba >> 24) & 0xFF;
    atapi_packet[ 3] = (lba >> 16) & 0xFF;
    atapi_packet[ 4] = (lba >> 8) & 0xFF;
    atapi_packet[ 5] = (lba >> 0) & 0xFF;
    atapi_packet[ 6] = 0x0;
    atapi_packet[ 7] = 0x0;
    atapi_packet[ 8] = 0x0;
    atapi_packet[ 9] = numsects;
    atapi_packet[10] = 0x0;
    atapi_packet[11] = 0x0;

    ide_write(channel, ATA_REG_HDDEVSEL, slavebit << 4); // Select the drive.

     /* Delay 400 nanoseconds for select to complete. */
    for(int i = 0; i < 4; i++)
        ide_read(channel, ATA_REG_ALTSTATUS); // Reading the Alternate Status port wastes 100ns.

    ide_write(channel, ATA_REG_FEATURES, 0); // PIO mode.

    ide_write(channel, ATA_REG_LBA1, (words * 2) & 0xFF); // Lower byte of sector size.
    ide_write(channel, ATA_REG_LBA2, (words * 2) >> 8); // Upper byte of sector size.

    ide_write(channel, ATA_REG_COMMAND, ATA_CMD_PACKET); // Send the command.
 
    if (err = ide_polling(channel, 1))
        return err; // Polling and return if error.
 
    __asm__("rep   outsw" : : "c"(6), "d"(bus), "S"(atapi_packet)); // Send packet data.

    /* Receiving data. */
    for (int i = 0; i < numsects; i++)
    {
        ide_wait_irq(); // Wait for an IRQ.
        if (err = ide_polling(channel, 1))
            return err; // Polling and return if error.
        io_insw(bus, edi, words);
        edi += (words * 2);
    }

    ide_wait_irq();
 
    /* Waiting for BSY & DRQ to clear. */
    while (ide_read(channel, ATA_REG_STATUS) & (ATA_SR_BSY | ATA_SR_DRQ))
        __asm__("nop");
 
    return 0; // Done !
}

void ide_read_sectors(uint8_t drive, uint8_t numsects, uint32_t lba, uint16_t es, uint32_t edi)
{
    if (drive > 3 || ide_devices[drive].reserved == 0) // Check if the drive is presents.
        return; // Drive not found !
    else if (((lba + numsects) > ide_devices[drive].size) && (ide_devices[drive].type == IDE_ATA)) // Check if inputs are valid.
        return; // Seeking to invalid position.
    else // Read in PIO Mode through Polling & IRQs.
    {
        uint8_t err;
        if (ide_devices[drive].type == IDE_ATA)
            err = ide_ata_access(ATA_READ, drive, lba, numsects, es, edi);
        else if (ide_devices[drive].type == IDE_ATAPI)
            for (int i = 0; i < numsects; i++)
                err = ide_atapi_read(drive, lba + i, 1, es, edi + (i * 2048));
        
        ide_print_error(drive, err);
    }
}

void ide_write_sectors(uint8_t drive, uint8_t numsects, uint32_t lba, uint16_t es, uint32_t edi)
{
    if (drive > 3 || ide_devices[drive].reserved == 0) // Check if the drive is presents.
        return; // Drive not found !
    else if (((lba + numsects) > ide_devices[drive].size) && (ide_devices[drive].type == IDE_ATA)) // Check if inputs are valid.
        return; // Seeking to invalid position.
    else // Read in PIO Mode through Polling & IRQs.
    {
        uint8_t err;
        if (ide_devices[drive].type == IDE_ATA)
            err = ide_ata_access(ATA_WRITE, drive, lba, numsects, es, edi);
        else if (ide_devices[drive].type == IDE_ATAPI)
            for (int i = 0; i < numsects; i++)
                err = ide_atapi_read(drive, lba + i, 1, es, edi + (i * 2048));
        
        ide_print_error(drive, err);
    }
}

void ide_atapi_eject(uint8_t drive)
{
    uint32_t   channel  = ide_devices[drive].channel;
    uint32_t   slavebit = ide_devices[drive].drive;
    uint32_t   bus      = channels[channel].base;
    uint32_t   words    = 2048 / 2; // Sector size in words.
    uint8_t    err      = 0;
    ide_irq_invoked     = 0;
    
    if (drive > 3 || ide_devices[drive].reserved == 0) // Check if the drive presents.
        return; // Drive not found !
    else if (ide_devices[drive].type == IDE_ATA) // Check if drive isn't ATAPI.
        return;  // Command Aborted.
    else // Eject ATAPI driver.
    {
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = ide_irq_invoked = 0x0); // Enable IRQs.
    
        /* Setup SCSI Packet. */
        atapi_packet[ 0] = ATAPI_CMD_EJECT;
        atapi_packet[ 1] = 0x00;
        atapi_packet[ 2] = 0x00;
        atapi_packet[ 3] = 0x00;
        atapi_packet[ 4] = 0x02;
        atapi_packet[ 5] = 0x00;
        atapi_packet[ 6] = 0x00;
        atapi_packet[ 7] = 0x00;
        atapi_packet[ 8] = 0x00;
        atapi_packet[ 9] = 0x00;
        atapi_packet[10] = 0x00;
        atapi_packet[11] = 0x00;
    
        /* Select the Drive. */
        ide_write(channel, ATA_REG_HDDEVSEL, slavebit << 4);
    
        /* Delay 400 nanosecond for select to complete. */
        for(int i = 0; i < 4; i++)
            ide_read(channel, ATA_REG_ALTSTATUS); // Reading alternate status port wastes 100ns.
    
        /* Send the packet command. */
        ide_write(channel, ATA_REG_COMMAND, ATA_CMD_PACKET); // Send the command.
    
        /* Waiting for the driver to finish or invoke an error. */
        err = ide_polling(channel, 1); // Polling and stop if error.
    
        /* Sending the packet data. */
        __asm__("rep outsw"::"c"(6), "d"(bus), "S"(atapi_packet)); // Send packet data.
        ide_wait_irq();  // Wait for an IRQ.
        err = ide_polling(channel, 1); // Polling and get error code.
        if (err == 3)
            err = 0; // DRQ is not needed here.

        ide_print_error(drive, err); // Return.
    }
}