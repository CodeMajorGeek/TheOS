#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

typedef struct
{
    uint8_t magic;
    char name[64];
    uint32_t offset;
    uint32_t length;
} initrd_header_t;

int main(char argc, char** argv)
{
    uint32_t nheaders = (argc - 1) / 2;
    initrd_header_t headers[64];

    printf("Size of header: %ld.\n", sizeof(initrd_header_t));

    uint32_t off = sizeof(initrd_header_t) * 64 + sizeof(int);
    for (int i = 0; i < nheaders; i++)
    {
        printf("Writing file %s -> %s at 0x%x.\n", argv[i * 2 + 1], argv[i * 2 + 2], off);
        strcpy(headers[i].name, argv[i * 2 + 2]);
        headers[i].offset = off;
        FILE* stream = fopen(argv[i * 2 + 1], "r");
        if (!stream)
        {
            printf("Error: file not found %s\n", argv[i * 2 + 1]);
            return 1;
        }
        fseek(stream, 0, SEEK_END);
        headers[i].length = ftell(stream);
        off += headers[i].length;
        fclose(stream);
        headers[i].magic = 0xBF;
    }

    FILE* wstream = fopen("./initrd.img", "w");
    uint8_t* data = (uint8_t*) malloc(off);
    fwrite(&nheaders, sizeof(int), 1, wstream);
    fwrite(headers, sizeof(initrd_header_t), 64, wstream);

    for (int i = 0; i < nheaders; i++)
    {
        FILE* stream = fopen(argv[i * 2 + 1], "r");
        uint8_t* buf = (uint8_t*) malloc(headers[i].length);
        fread(buf, 1, headers[i].length, stream);
        fwrite(buf, 1, headers[i].length, wstream);
        fclose(stream);
        free(buf);
    }

    fclose(wstream);
    free(data);

    return 0;
}