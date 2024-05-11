#include "../header/commands/commands.h"


void putchar(char character, uint32_t color) {
    uint16_t position = framebuffer_get_cursor();
    
    uint8_t row = position / RESOLUTION_WIDTH;

    uint8_t col = position - row * RESOLUTION_WIDTH;
    
    framebuffer_write(row, col, character, color, 0);
    framebuffer_set_cursor(row, col + 1);
}

void puts(char* buffer, uint32_t count, uint32_t color) {
    uint32_t i = 0;
    char c;
    uint8_t row = 0; uint8_t col = 0;
    while (i < count)
    {   
        c = buffer[i];
        framebuffer_write(row, col, c, color, 0);
        if (col >= RESOLUTION_WIDTH)
        {
            ++row;
            col = 0;
        } else
        {
            ++col;
        }
        framebuffer_set_cursor(row, col);
        i++;
    }
}