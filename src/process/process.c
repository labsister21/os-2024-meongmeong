#include "../header/process/process.h"
#include "../header/memory/paging.h"
#include "../header/stdlib/string.h"
#include "../header/cpu/gdt.h"

static struct ProcessManagerState process_manager_state = { 
    .active_process_count = 0 
};

struct ProcessControlBlock* _process_list[PROCESS_COUNT_MAX] = {0};

// int32_t process_list_get_inactive_index()
// {
//     for (int i = 0; i < PROCESS_COUNT_MAX ; i++)
//     {
//         if (&_process_list[i]->metadata.state == PROCESS_STATE_READY)
//         {
//             return i;
//         }
//     }

//     return -1; // full
// }
// ganti ke manager buat kyk paging aja


uint32_t ceil_div(uint32_t a, uint32_t b) {
    return (a + b - 1) / b;
}

int32_t process_create_user_process(struct FAT32DriverRequest request) {

    // 1. step validation and failure condition checks
    int32_t retcode = PROCESS_CREATE_SUCCESS;  // 0
    if (process_manager_state.active_process_count >= PROCESS_COUNT_MAX) {
        retcode = PROCESS_CREATE_FAIL_MAX_PROCESS_EXCEEDED; // 1
        goto exit_cleanup;
    }

    // Ensure entrypoint is not located at kernel's section at higher half
    if ((uint32_t) request.buf >= KERNEL_VIRTUAL_ADDRESS_BASE) {
        retcode = PROCESS_CREATE_FAIL_INVALID_ENTRYPOINT; //2
        goto exit_cleanup;
    }

    // Check whether memory is enough for the executable and additional frame for user stack
    uint32_t page_frame_count_needed = ceil_div(request.buffer_size + PAGE_FRAME_SIZE, PAGE_FRAME_SIZE);
    if (!paging_allocate_check(page_frame_count_needed) || page_frame_count_needed > PROCESS_PAGE_FRAME_COUNT_MAX) {
        retcode = PROCESS_CREATE_FAIL_NOT_ENOUGH_MEMORY; // 3
        goto exit_cleanup;
    }

    // Process PCB [ini kykyna hrs di lanjutin]
    int32_t p_index = process_list_get_inactive_index();
    struct ProcessControlBlock *new_pcb = &(_process_list[p_index]);

    new_pcb->metadata.pid = process_generate_new_pid();


    // steps yang blom dilakukan 
    // 2. load excevutable into memory
    read(request);
    // 3. set up initial state dan context
    new_pcb->metadata.state = PROCESS_STATE_RUNNING;

    // 4. siapin state dan context awala program
    /// 5. catet informasi penting prcoess ke metadata
    // 6. mengembalikan semua state register dan memeory ke awal
exit_cleanup:
    return retcode;
}