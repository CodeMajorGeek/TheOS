#include <Kernel/CPU/io.h>

void io_outb(uint16_t port, uint8_t value)
{
  __asm__("outb %1, %0" : : "d"(port), "a" (value));
}

uint8_t io_inb(uint16_t port)
{
	unsigned char ret;
	__asm__("inb %1, %0" : "=a" (ret) : "d" (port));
	return ret;
}
