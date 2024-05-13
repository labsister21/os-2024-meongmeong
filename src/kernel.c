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
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    initialize_filesystem_fat32();
    gdt_install_tss();
    set_tss_register();

    // Allocate first 4 MiB virtual memory
    paging_allocate_user_page_frame(&_paging_kernel_page_directory, (uint8_t*) 0);

    
    // Write shell into memory
    struct FAT32DriverRequest request = {
        .buf                   = (uint8_t*) 0,
        .name                  = "shell",
        .ext                   = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = 0x100000,
    };
    read(request);
    
    struct FAT32DriverRequest request1 = {
        .buf                   = "Hello world",
        .name                  = "nunu",
        .ext                   = "txt",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = CLUSTER_SIZE,
    };
    write(request1);
    // framebuffer_write(0,0,stat-'0',0xFF,0x00);
    // Set TSS $esp pointer and jump into shell 
    set_tss_kernel_current_stack();
    kernel_execute_user_program((uint8_t*) 0);
    framebuffer_write(0,1,'x', 0xff, 0x00);
    // Write nunu.txt into memory
    while (true);
}
