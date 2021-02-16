#include <string.h>

int memcmp(const void* aptr, const void* bptr, size_t size)
{
    const unsigned char* a = (const unsigned char*) aptr;
    const unsigned char* b = (const unsigned char*) bptr;
    for (size_t i = 0; i < size; i++)
    {
        if (a[i] < b[i])
            return -1;
        else if (a[i] > b[i])
            return 1;
    }
    return 0;
}

void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size)
{
    unsigned char* dst = (unsigned char*) dstptr;
    const unsigned char* src = (const unsigned char*) srcptr;
    for (size_t i = 0; i < size; i++)
        dst[i] = src[i];
    return dstptr;
}

void* memmove(void* dstptr, const void* srcptr, size_t size)
{
    unsigned char* dst = (unsigned char*) dstptr;
    const unsigned char* src = (const unsigned char*) srcptr;
    if (dst < src)
        for (size_t i = 0; i < size; i++)
            dst[i] = src[i];
    else
        for (size_t i = size; i; i--)
            dst[i - 1] = src[i - 1];
    return dstptr;
}

void* memset(void* bufptr, int value, size_t size)
{
    uint8_t* buf = (uint8_t*) bufptr;
    for (size_t i = 0; i < size; i++)
        buf[i] = (uint8_t) value;
    return bufptr;
}

void* memsetw(void* bufptr, int value, size_t size)
{
    uint16_t* buf = (uint16_t*) bufptr;
    for (size_t i = 0; i < size; i++)
        buf[i] = (uint16_t) value;
    return bufptr;
}

size_t strlen(const char* str)
{
    size_t len = 0;
    while(str[len])
        len++;
    return len;
}