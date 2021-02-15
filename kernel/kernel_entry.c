
void write_string(int color, const char *str)
{
    volatile char *video = (volatile char *) 0xB8000;
    while (*str != 0)
    {
        *video++ = *str++;
        *video++ = color;
    }
}

void k_entry(void)
{

    write_string(15, "TESSSSSSSSSSSSSSSSSSSSSST !");

    for(;;);
}