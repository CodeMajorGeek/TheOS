__asm__ (".code16gcc\n");

#include <stdint.h>

typedef struct SMAP_entry
{
    uint64_t Base; // Base address QWORD
    uint64_t Length; // Length QWORD
    uint32_t Type; // Entry Type
    uint32_t ACPI; // Extended
} __attribute__((packed)) SMAP_entry_t;

int __attribute__((noinline)) __attribute__((regparm(3))) detectMemory(SMAP_entry_t* buffer, int max_entries)
{
    uint32_t countID = 0;
    int entries = 0, signature, bytes;
    do
    {
        __asm__ __volatile__ ("int $0x15"
                                : "=a"(signature), "=c"(bytes), "=b"(countID)
                                : "a"(0xE820), "b"(countID), "c"(24), "d"(0x534D4150), "D"(buffer));
        if (signature != 0x534D4150)
            return -1;
        if (bytes > 20 && (buffer->ACPI & 0x0001) == 0)
        {

        }
        else
        {
            buffer++;
            entries++;
        }
    }
    while (countID != 0 && entries < max_entries);
    return entries;
}