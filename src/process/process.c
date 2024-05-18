#include "header/process/process.h"
#include "header/memory/paging.h"
#include "header/stdlib/string.h"
#include "header/cpu/gdt.h"
#include "header/scheduler/scheduler.h"

static struct ProcessManagerState process_manager_state = {
    .process_map = {[0 ... PROCESS_COUNT_MAX - 1] = false},
    .active_process_count = 0};

struct ProcessControlBlock _process_list[PROCESS_COUNT_MAX] = {0};

int32_t process_list_get_inactive_index()
{
    for (int i = 0; i < PROCESS_COUNT_MAX; i++)
    {
        if (process_manager_state.process_map[i] == false)
        {
            return i;
        }
    }
    return -1; // full
}

uint32_t process_generate_new_pid()
{
    int32_t id = process_list_get_inactive_index();
    if (id >= 0)
    {
        process_manager_state.process_map[id] = true;
        process_manager_state.active_process_count++;
        return id + 1;
    }
    return 0;
}

struct ProcessControlBlock *process_get_current_running_pcb_pointer(void)
{
    for (int i = 0; i < PROCESS_COUNT_MAX; i++)
    {

        if (_process_list[i].metadata.state == PROCESS_STATE_RUNNING)
        {
            return &_process_list[i];
        }
    }
    return NULL;
}

uint32_t ceil_div(uint32_t a, uint32_t b)
{
    return (a + b - 1) / b;
}

int32_t process_create_user_process(struct FAT32DriverRequest request)
{

    // 1. step validation and failure condition checks
    int32_t retcode = PROCESS_CREATE_SUCCESS; // 0
    if (process_manager_state.active_process_count >= PROCESS_COUNT_MAX)
    {
        retcode = PROCESS_CREATE_FAIL_MAX_PROCESS_EXCEEDED; // 1
        goto exit_cleanup;
    }

    // Ensure entrypoint is not located at kernel's section at higher half
    if ((uint32_t)request.buf >= KERNEL_VIRTUAL_ADDRESS_BASE)
    {
        retcode = PROCESS_CREATE_FAIL_INVALID_ENTRYPOINT; // 2
        goto exit_cleanup;
    }

    // Check whether memory is enough for the executable and additional frame for user stack
    uint32_t page_frame_count_needed = ceil_div(request.buffer_size + PAGE_FRAME_SIZE, PAGE_FRAME_SIZE);
    if (!paging_allocate_check(page_frame_count_needed) || page_frame_count_needed > PROCESS_PAGE_FRAME_COUNT_MAX)
    {
        retcode = PROCESS_CREATE_FAIL_NOT_ENOUGH_MEMORY; // 3
        goto exit_cleanup;
    }

    // Process PCB [ini kykyna hrs di lanjutin]
    int32_t p_index = process_list_get_inactive_index();
    struct ProcessControlBlock *new_pcb = &(_process_list[p_index]);

    new_pcb->metadata.pid = process_generate_new_pid();

    // steps yang blom dilakukan

    struct PageDirectory *old_page_dir = paging_get_current_page_directory_addr();
    struct PageDirectory *new_page_dir = paging_create_new_page_directory();

    new_pcb->context.page_directory_virtual_addr = new_page_dir;
    paging_allocate_user_page_frame(new_page_dir, request.buf);

    paging_use_page_directory(new_pcb->context.page_directory_virtual_addr);

    retcode = read(request);

    // 2. load excevutable into memory
    if (retcode != 0)
    {
        retcode = PROCESS_CREATE_FAIL_FS_READ_FAILURE;
        goto exit_cleanup;
    }

    new_pcb->metadata.state = PROCESS_STATE_READY;
    new_pcb->context.eflags |= CPU_EFLAGS_BASE_FLAG | CPU_EFLAGS_FLAG_INTERRUPT_ENABLE;

    new_pcb->context.eip = (uint32_t)request.buf;
    new_pcb->context.cpu.index.edi = 0;
    new_pcb->context.cpu.index.esi = 0;

    new_pcb->context.cpu.stack.ebp = 0xBFFFFFFC;
    new_pcb->context.cpu.stack.esp = 0xBFFFFFFC;

    new_pcb->context.cpu.general.ebx = 0;
    new_pcb->context.cpu.general.edx = 0;
    new_pcb->context.cpu.general.edx = 0;
    new_pcb->context.cpu.general.eax = 0;

    new_pcb->context.cpu.segment.gs = GDT_USER_DATA_SEGMENT_SELECTOR;
    new_pcb->context.cpu.segment.fs = GDT_USER_DATA_SEGMENT_SELECTOR;
    new_pcb->context.cpu.segment.es = GDT_USER_DATA_SEGMENT_SELECTOR;
    new_pcb->context.cpu.segment.ds = GDT_USER_DATA_SEGMENT_SELECTOR;

    pcbqueue_enque(&pcb_queue, new_pcb);

    paging_use_page_directory(old_page_dir);

exit_cleanup:
    return retcode;
}