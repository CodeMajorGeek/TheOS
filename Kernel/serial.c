#include <Kernel/serial.h>

bool serial_init(uint16_t serial)
{
    io_outb(serial + 1, 0x00); // Disable all interrupts.
    io_outb(serial + 3, 0x80); // Enable DLAB (set baud rate divisor).
    io_outb(serial + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud.
    io_outb(serial + 1, 0x00); // Set divisor to 3 (hi byte) 38400 baud.
    io_outb(serial + 3, 0x03); // 8 bits, no parity, one stop bit.
    io_outb(serial + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold.
    io_outb(serial + 4, 0x0B); // IRQs enabled, RTS/DSR set.
    io_outb(serial + 4, 0x1E); // Set in loopback mode, test the serial chip.
    io_outb(serial + 0, 0xAE); // Test serial chip (send byte 0xAE and check if serial returns same byte).

    // Check if serial is faulty.
    if (io_inb(serial + 0) != 0xAE)
        return false;

    // If not, set it in normal operation mode. (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled).
    io_outb(serial + 4, 0x0F);
    return true;
}

bool serial_received(uint16_t serial)
{
    return io_inb(serial + 5) & 1;
}

char serial_read(uint16_t serial)
{
    while (!serial_received(serial));
    return io_inb(serial);
}

bool serial_is_transmit_empty(uint16_t serial)
{
    return io_inb(serial + 5) & 0x20;
}

void serial_write(uint16_t serial, char a)
{
    while (!serial_is_transmit_empty(serial));
    io_outb(serial, a);
}

void serial_puts(uint16_t serial, const char* str)
{
    while (*str)
        serial_write(serial, *str++);
}