#include <config/keyboard.h>

const char keycodes[] = 
{
    0, 0, '&', 0, '"', '\'', '(', '-', 0, '_', 0, 0, ')', '=', 0,
    0, 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',
    0, 'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 0, 0, 0, '*',
    'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', '!', 0,
    0, 0, ' '

};

char scancode_to_ASCII(uint8_t sc)
{
    if (sc < sizeof(keycodes))
        return keycodes[sc];
    return NULL;
}