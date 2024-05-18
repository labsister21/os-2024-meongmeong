#include "../header/kernelutils/kernelutils.h"

void putchar(char character, uint32_t color)
{
    uint16_t position = framebuffer_get_cursor();
    uint8_t row = position / RESOLUTION_WIDTH;
    uint8_t col = position - row * RESOLUTION_WIDTH;

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

    if (row <= RESOLUTION_HEIGHT - 1 && col <= RESOLUTION_WIDTH - 1)
    {
        framebuffer_write(row, col, character, color, 0);
        framebuffer_set_cursor(row, col + 1);
    }
    else
    {
        framebuffer_clear();
        framebuffer_set_cursor(0, 0);
        framebuffer_write(0, 0, character, color, 0);
        framebuffer_set_cursor(0, 1);
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

char *int_to_string(int num)
{
    static char str[11];
    int i = 0;
    if (num == 0)
    {
        str[i++] = '0';
    }
    else
    {
        if (num < 0)
        {
            str[i++] = '-';
            num = -num;
        }
        int j = i;
        while (num > 0)
        {
            str[i++] = num % 10 + '0';
            num /= 10;
        }
        // Reverse the string
        int k = i - 1;
        while (j < k)
        {
            char temp = str[j];
            str[j] = str[k];
            str[k] = temp;
            j++;
            k--;
        }
    }
    str[i] = '\0';
    return str;
}