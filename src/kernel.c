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
#include "header/memory/paging.h"

void kernel_setup(void) {
    load_gdt(&_gdt_gdtr);
    initialize_filesystem_fat32();
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    
    paging_allocate_user_page_frame(&_paging_kernel_page_directory, (uint8_t *)0x500000);
    *((uint8_t*) 0x500000) = 1;
    int col = 0;
    keyboard_state_activate();
    while (true) {
         char c;
         get_keyboard_buffer(&c);
         if (c) framebuffer_write(0, col++, c, 0xF, 0);
    }
}