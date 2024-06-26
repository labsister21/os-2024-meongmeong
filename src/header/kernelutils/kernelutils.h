#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "stdint.h"
#include "../driver/framebuffer.h"
#include "../stdlib/string.h"

void putchar(char character, uint32_t color);

void puts(char *buffer, uint32_t count, uint32_t color);
char *int_to_string(int num);
void put_clock(const char *hours, const char *minutes, const char *seconds);

#endif