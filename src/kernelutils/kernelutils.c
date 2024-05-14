#include "../header/kernelutils/kernelutils.h"

void putchar(char character, uint32_t color)
{
    uint16_t position = framebuffer_get_cursor();
    uint8_t row = position / RESOLUTION_WIDTH;
    uint8_t col = position - row * RESOLUTION_WIDTH;

    int repeat = 0;

    if (memcmp(&character, "\n", 1) == 0)
    {
        framebuffer_set_cursor(row + 1, 0);
        return;
    }
    else if (memcmp(&character, "\b", 1) == 0)
    {
        position--;
        uint8_t row = position / RESOLUTION_WIDTH;
        uint8_t col = position - row * RESOLUTION_WIDTH;
        framebuffer_write(row, col, ' ', color, 0);
        framebuffer_set_cursor(row, col);
        return;
    }

    if (row <= RESOLUTION_HEIGHT-1  && col <= RESOLUTION_WIDTH -1 )
    {
        framebuffer_write(row, col, character, color, 0);
        framebuffer_set_cursor(row, col + 1);   
    }
    else {
        framebuffer_clear();
        framebuffer_set_cursor(0, 0);
        framebuffer_write(0, 0, character, color, 0);
        framebuffer_set_cursor(0,1);
    }
}

void puts(char *buffer, uint32_t count, uint32_t color)
{
    uint32_t i = 0;
    char c;
    while (i < count)
    {
        c = buffer[i];
        putchar(c, color);
        i++;
    }
}
