#include <stdint.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
#include "header/kernel-entrypoint.h"
#include "header/driver/framebuffer.h"
#include "header/cpu/idt.h"
#include "header/cpu/interrupt.h"
#include "header/driver/keyboard.h"
#include "header/filesystem/fat32.h"
#include "header/driver/disk.h"

void kernel_setup(void)
{
    load_gdt(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);

    struct BlockBuffer b;
    for (int i = 0; i < 512; i++)
        b.buf[i] = i % 16;
    write_blocks(&b, 17, 1);
    while (true)
        ;

    // int col = 0;
    // keyboard_state_activate();
    // while (true) {
    //      char c;
    //      get_keyboard_buffer(&c);
    //      if (c) framebuffer_write(0, col++, c, 0xF, 0);
    // }
}
