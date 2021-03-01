#include <sys/syscall.h>

int syscall(int type, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5)
{
    int a;
    __asm__ __volatile__("int $0x80" : "=a"(a) : "0"(type), "b"((int) p1), "c"((int) p2), "d"((int) p3), "S"((int) p4), "D"((int) p5));
    return a;
}