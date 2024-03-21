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

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg)
{
    // TODO : Implement
}

void framebuffer_clear(void)
{
    // TODO : Implement
}