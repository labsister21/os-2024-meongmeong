#include "../header/kernelutils/kernelutils.h"
#include "../header/driver/framebuffer.h"

void putchar(char character, uint32_t color)
{
    uint16_t position = framebuffer_get_cursor();

    uint8_t row = position / RESOLUTION_WIDTH;

    uint8_t col = position - row * RESOLUTION_WIDTH;

    framebuffer_write(row, col, character, color, 0);
    framebuffer_set_cursor(row, col + 1);
}

void puts(char *buffer, uint32_t count, uint32_t color)
{
    uint32_t i = 0;
    char c;
    uint16_t position = framebuffer_get_cursor();

    uint8_t row = position / RESOLUTION_WIDTH;

    uint8_t col = position - row * RESOLUTION_WIDTH;

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

void puts_with_newline(char *buffer, uint32_t count, uint32_t color)
{
    puts(buffer, count, color);
    putchar('\n', color);
}