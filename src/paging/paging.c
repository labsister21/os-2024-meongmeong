
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../header/memory/paging.h"

__attribute__((aligned(0x1000))) struct PageDirectory _paging_kernel_page_directory = {
    .table = {
        [0] = {
            .flag.present_bit       = 1,
            .flag.write_bit         = 1,
            .flag.use_pagesize_4_mb = 1,
            .lower_address          = 0,
        },
        [0x300] = {
            .flag.present_bit       = 1,
            .flag.write_bit         = 1,
            .flag.use_pagesize_4_mb = 1,
            .lower_address          = 0,
        },
    }
};

static struct PageManagerState page_manager_state = {
    .page_frame_map = {
        [0]                            = true,
        [1 ... PAGE_FRAME_MAX_COUNT-1] = false
    }, 
    .free_page_frame_count = PAGE_FRAME_MAX_COUNT -1,
    // TODO: Initialize page manager state properly
};

void update_page_directory_entry(
    struct PageDirectory *page_dir,
    void *physical_addr, 
    void *virtual_addr, 
    struct PageDirectoryEntryFlag flag
) {
    uint32_t page_index = ((uint32_t) virtual_addr >> 22) & 0x3FF;
    page_dir->table[page_index].flag          = flag;
    page_dir->table[page_index].lower_address = ((uint32_t) physical_addr >> 22) & 0x3FF;
    flush_single_tlb(virtual_addr);
}

void flush_single_tlb(void *virtual_addr) {
    asm volatile("invlpg (%0)" : /* <Empty> */ : "b"(virtual_addr): "memory");
}



/* --- Memory Management --- */
// TODO: Implement
bool paging_allocate_check(uint32_t amount) {
    // TODO: Check whether requested amount is available
    if (page_manager_state.free_page_frame_count >= amount){
        return true;
    }
    return false;
}


bool paging_allocate_user_page_frame(struct PageDirectory *page_dir, void *virtual_addr) {
    /*
     * TODO: Find free physical frame and map virtual frame into it
     * - Find free physical frame in page_manager_state.page_frame_map[] using any strategies
     * - Mark page_manager_state.page_frame_map[]
     * - Update page directory with user flags:
     *     > present bit    true
     *     > write bit      true
     *     > user bit       true
     *     > pagesize 4 mb  true
     */ 

    // using first-fit algorithm
    if (page_manager_state.free_page_frame_count > 0){

        for (int i = 0;i < PAGE_FRAME_MAX_COUNT;i++)
        {
            if (!page_manager_state.page_frame_map[i])
            {
                // mark the page
                page_manager_state.page_frame_map[i] = true;
                page_manager_state.free_page_frame_count--; // subtract free page


                void *physical_addr = (void *)(i * PAGE_FRAME_SIZE); 

                // update the page directory entry
                update_page_directory_entry(page_dir, physical_addr, virtual_addr, (struct PageDirectoryEntryFlag){
                    .present_bit       = 1,
                    .write_bit         = 1,
                    .user_supervisor_bit = 1,
                    .use_pagesize_4_mb = 1
                });
            }
        }

        return true;
    }
    return false; // full
}

bool paging_free_user_page_frame(struct PageDirectory *page_dir, void *virtual_addr) {
    /* 
     * - Use the page_dir.table values to check mapped physical frame
     * - Remove the entry by setting it into 0
     */

    
    // index of page directory table
    uint32_t page_index = ((uint32_t) virtual_addr >> 22) & 0x3FF;

    struct PageDirectoryEntry *entry = &page_dir->table[page_index];

    if (entry->flag.present_bit)
    {
        // getting physical address
        uint32_t physical_addr = ((uint32_t)entry->higher_address << 30) | ((uint32_t)entry->lower_address << 12);

        // index frame
        uint32_t frame_index = physical_addr/PAGE_FRAME_SIZE;

        // if index valid
        if (frame_index < PAGE_FRAME_MAX_COUNT) {
            page_manager_state.page_frame_map[frame_index] = false;
            page_manager_state.free_page_frame_count++;

            // deallocat entry
            entry->flag.present_bit = 0;
            entry->flag.user_supervisor_bit = 0;
            entry->flag.write_bit = 0;
            entry->higher_address = 0; 
            entry->lower_address = 0;  
            flush_single_tlb(virtual_addr);

            return true; // successful
        }


    }    
    return false; // failed
}
