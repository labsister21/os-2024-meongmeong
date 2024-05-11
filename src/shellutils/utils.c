#include "../../header/shellutils/utils.h"

void putchar(char character, uint32_t color)
{
    framebuffer_write(0, 0, character, color, 0);
    framebuffer_set_cursor(0, 1);
}

void puts(char *buffer, uint32_t count, uint32_t color)
{
    uint32_t i = 0;
    char c;
    uint8_t row = 0;
    uint8_t col = 0;
    while (i < count)
    {
        c = buffer[i];
        framebuffer_write(row, col, c, color, 0);
        if (col >= RESOLUTION_WIDTH)
        {
            ++row;
            col = 0;
        }
        else
        {
            ++col;
        }
        framebuffer_set_cursor(row, col);
        i++;
    }
}