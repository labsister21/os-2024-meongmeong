#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../header/driver/framebuffer.h"
#include "../header/stdlib/string.h"
#include "../header/cpu/portio.h"

void framebuffer_set_cursor(uint8_t r, uint8_t c)
{
    // TODO : Implement
    uint16_t pos = r * RESOLUTION_WIDTH + c;
	out(CURSOR_PORT_CMD, 0x0F);
	out(CURSOR_PORT_DATA, (uint8_t) (pos & 0xFF));
	out(CURSOR_PORT_CMD, 0x0E);
	out(CURSOR_PORT_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}

uint16_t framebuffer_get_cursor(void) {
    out(CURSOR_PORT_CMD, 0x0F);
    uint16_t pos = in(CURSOR_PORT_DATA);
    out(CURSOR_PORT_CMD, 0x0E);
    pos |= (((uint16_t) in(CURSOR_PORT_DATA)) << 8) & 0xFF00;

   return pos;
}

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg)
{
    uint16_t idx = (row * RESOLUTION_WIDTH + col) * 2;
    FRAMEBUFFER_MEMORY_OFFSET[idx] = c;
    FRAMEBUFFER_MEMORY_OFFSET[idx + 1] = (bg << 4) | fg;
}

void framebuffer_clear(void)
{
    for (uint16_t i = 1; i < 80 * 25 * 2; i++)
    {
        if (i % 2 == 0)
        {
            // character
            FRAMEBUFFER_MEMORY_OFFSET[i] = 0x00;
        }
        else
        {
            // color
            FRAMEBUFFER_MEMORY_OFFSET[i] = 0x07;
        }
    }
}