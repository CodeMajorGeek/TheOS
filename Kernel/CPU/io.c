#include <Kernel/CPU/io.h>

void io_outb(uint16_t port, uint8_t value)
{
  __asm__ __volatile__("outb %1, %0" : : "d"(port), "a" (value));
}

uint8_t io_inb(uint16_t port)
{
	uint8_t ret;
	__asm__ __volatile__("inb %1, %0" : "=a" (ret) : "d" (port));
	return ret;
}

uint32_t io_inl(uint16_t port)
{
	uint32_t ret;
	__asm__ __volatile__("inl %1, %0" : "=a" (ret) : "d" (port));
	return ret;
}

void io_outsw(uint32_t port, uint32_t buffer, uint32_t count)
{
	__asm__ __volatile__ ("cld; rep; outsw" :: "D" (buffer), "d" (port), "c" (count));
}

void io_insl(uint32_t port, uint32_t* buffer, uint32_t count)
{
	__asm__ __volatile__ ("cld; rep; insl" :: "D" (buffer), "d" (port), "c" (count));
}

void io_insw(uint32_t port, uint32_t buffer, uint32_t count)
{
	__asm__ __volatile__ ("cld; rep; insw" :: "D" (buffer), "d" (port), "c" (count));
}
