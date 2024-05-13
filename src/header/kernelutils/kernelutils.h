#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "stdint.h"
#include "../driver/framebuffer.h"
#include "../stdlib/string.h"

void putchar(char character, uint32_t color);

void puts(char *buffer, uint32_t count, uint32_t color);

#endif