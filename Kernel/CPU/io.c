#include <Kernel/CPU/io.h>

void io_outb(uint16_t port, uint8_t value)
{
  __asm__("outb %1, %0" : : "d"(port), "a" (value));
}

uint8_t io_inb(uint16_t port)
{
	uint8_t ret;
	__asm__("inb %1, %0" : "=a" (ret) : "d" (port));
	return ret;
}

uint32_t io_inl(uint16_t port)
{
	uint32_t ret;
	__asm__("inl %1, %0" : "=a" (ret) : "d" (port));
	return ret;
}

void io_insl(uint16_t port, uint32_t* buffer, uint32_t count)
{
	for (uint32_t i = 0; i < count; i++)
		buffer[i] = io_inl(port);
}