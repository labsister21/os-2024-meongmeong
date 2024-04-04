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

    initialize_filesystem_fat32();
    struct FAT32DriverRequest request = {
        .name = "root\0\0\0\0",
        .ext = "\0",
        .parent_cluster_number = 2,
        .buf = (uint8_t*) 0,
        .buffer_size = 10000 // Removed semicolon at the end of this line
    };

    read_directory(request);

    // int col = 0;
    // keyboard_state_activate();
    // while (true) {
    //      char c;
    //      get_keyboard_buffer(&c);
    //      if (c) framebuffer_write(0, col++, c, 0xF, 0);
    // }
}
