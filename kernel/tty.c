#include <kernel/tty.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) DISPLAY_ADDRESS; 

static size_t tty_row;
static size_t tty_col;
static uint8_t tty_color;
static uint16_t* tty_buffer;

void tty_init(void)
{
    tty_color = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);
    tty_buffer = VGA_MEMORY;
    tty_clear();
}

void tty_set_color(uint8_t color)
{
    tty_color = color;
}

void tty_put_entry_at(unsigned char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    tty_buffer[index] = vga_entry(c, color);
}

void tty_putc(char c)
{
    unsigned char uc = (unsigned char) c;
    tty_put_entry_at(uc, tty_color, tty_col, tty_row);
    if (++tty_col == VGA_WIDTH)
    {
        tty_col = 0;
        if (++tty_row == VGA_HEIGHT)
            tty_row = 0;
    }
}

void tty_write(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        tty_putc(data[i]);
}

void tty_puts(const char* s)
{
    tty_write(s, strlen(s));
}

void tty_clear(void)
{
    tty_col = 0;
    tty_row = 0;
    memsetw(tty_buffer, vga_entry(' ', tty_color), VGA_WIDTH * VGA_HEIGHT);
}