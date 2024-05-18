#include "../header/scheduler/scheduler.h"
#include "header/kernel-entrypoint.h"
struct PCBQueue pcb_queue = {0};
/* --- Scheduler --- */
/**
 * Initialize scheduler before executing init process
 */
// TODO
void scheduler_init(void)
{

    struct ProcessControlBlock *new_pcb = pcbqueue_deque(&pcb_queue);
    // Switch the page directory to that of the next process
    paging_use_page_directory(new_pcb->context.page_directory_virtual_addr);

    pcbqueue_enque(&pcb_queue, new_pcb);

    // Execute the next process's program
    kernel_execute_user_program((void *)new_pcb->context.eip);
    activate_timer_interrupt();
}

/**
 * Save context to current running process
 *
 * @param ctx Context to save to current running process control block
 */
// TODO
void scheduler_save_context_to_current_running_pcb(struct Context ctx)
{
    struct ProcessControlBlock *temp_pcb = process_get_current_running_pcb_pointer();
    temp_pcb->context = ctx;
}

/**
 * Trigger the scheduler algorithm and context switch to new process
 */
// TODO
__attribute__((noreturn)) void scheduler_switch_to_next_process(void)
{
    // Get the current running process
    struct ProcessControlBlock *current_pcb = process_get_current_running_pcb_pointer();

    // Dequeue the next process to run
    struct ProcessControlBlock *next_pcb = pcbqueue_deque(&pcb_queue);

    current_pcb->metadata.state = PROCESS_STATE_READY;
    next_pcb->metadata.state = PROCESS_STATE_RUNNING;
    // Enqueue the current process back to the queue

    pcbqueue_enque(&pcb_queue, current_pcb);

    // Switch the page directory to that of the next process
    paging_use_page_directory(next_pcb->context.page_directory_virtual_addr);

    // Execute the next process's program
    process_context_switch(next_pcb->context);
}