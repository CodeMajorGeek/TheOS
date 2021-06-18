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
    char ret = NULL;
    if (sc < sizeof(keycodes))
        ret = keycodes[sc];

    if (keyboard_is_uppercase()) // Make CAPLOCK & shift work :) (TODO: maybe move this stuff to kernel keyboard codes...).
        if (ret >= 'a' && ret <= 'z')
            ret -= 32;
        else if (sc >= 2 && sc <= 10)
            ret = '1' + (sc - 2);
        else if (sc == 11)
            ret = '0';

    return ret;
}