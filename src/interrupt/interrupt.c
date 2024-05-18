#include "header/cpu/interrupt.h"

#include "header/cpu/portio.h"
#include "header/cpu/idt.h"
#include "header/driver/keyboard.h"
#include "header/filesystem/fat32.h"
#include "header/kernelutils/kernelutils.h"
#include "header/scheduler/scheduler.h"
#include "header/stdlib/string-lib.h"

struct TSSEntry _interrupt_tss_entry = {
    .ss0 = GDT_KERNEL_DATA_SEGMENT_SELECTOR,
};

void io_wait(void)
{
    out(0x80, 0);
}

void pic_ack(uint8_t irq)
{
    if (irq >= 8)
        out(PIC2_COMMAND, PIC_ACK);
    out(PIC1_COMMAND, PIC_ACK);
}

void pic_remap(void)
{
    // Starts the initialization sequence in cascade mode
    out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    out(PIC1_DATA, PIC1_OFFSET); // ICW2: Master PIC vector offset
    io_wait();
    out(PIC2_DATA, PIC2_OFFSET); // ICW2: Slave PIC vector offset
    io_wait();
    out(PIC1_DATA, 0b0100); // ICW3: tell Master PIC, slave PIC at IRQ2 (0000 0100)
    io_wait();
    out(PIC2_DATA, 0b0010); // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    out(PIC1_DATA, ICW4_8086);
    io_wait();
    out(PIC2_DATA, ICW4_8086);
    io_wait();

    // Disable all interrupts
    out(PIC1_DATA, PIC_DISABLE_ALL_MASK);
    out(PIC2_DATA, PIC_DISABLE_ALL_MASK);
}

void main_interrupt_handler(struct InterruptFrame frame)
{
    switch (frame.int_number)
    {
    case IRQ_TIMER + PIC1_OFFSET:
        struct Context curr_context;
        curr_context.cpu.general.eax = frame.cpu.general.eax;
        curr_context.cpu.general.ebx = frame.cpu.general.ebx;
        curr_context.cpu.general.ecx = frame.cpu.general.ecx;
        curr_context.cpu.general.edx = frame.cpu.general.edx;
        curr_context.cpu.index.esi = frame.cpu.index.esi;
        curr_context.cpu.index.edi = frame.cpu.index.edi;
        curr_context.cpu.stack.ebp = frame.cpu.stack.ebp;
        curr_context.cpu.stack.esp = frame.cpu.stack.esp;
        curr_context.cpu.segment.ds = frame.cpu.segment.ds;
        curr_context.cpu.segment.es = frame.cpu.segment.es;
        curr_context.cpu.segment.fs = frame.cpu.segment.fs;
        curr_context.cpu.segment.gs = frame.cpu.segment.gs;
        curr_context.eflags = frame.int_stack.eflags;
        curr_context.eip = frame.int_stack.eip;
        curr_context.page_directory_virtual_addr = paging_get_current_page_directory_addr();

        // Save current context
        scheduler_save_context_to_current_running_pcb(curr_context);

        scheduler_switch_to_next_process();
        pic_ack(IRQ_TIMER + PIC1_OFFSET);
        break;
    case IRQ_KEYBOARD + PIC1_OFFSET:
        keyboard_isr();
        break;
    case 0x30:
        syscall(frame);
    }
}

void activate_keyboard_interrupt(void)
{
    out(PIC1_DATA, in(PIC1_DATA) & ~(1 << IRQ_KEYBOARD));
}

void set_tss_kernel_current_stack(void)
{
    uint32_t stack_ptr;
    // Reading base stack frame instead esp
    __asm__ volatile("mov %%ebp, %0" : "=r"(stack_ptr) : /* <Empty> */);
    // Add 8 because 4 for ret address and other 4 is for stack_ptr variable
    _interrupt_tss_entry.esp0 = stack_ptr + 8;
}

void syscall(struct InterruptFrame frame)
{
    switch (frame.cpu.general.eax)

    {
    // Syscall 0 = read
    case 0:
        *((int8_t *)frame.cpu.general.ecx) = read(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;
    // Syscall 1 = read_directory
    case 1:
        *((int8_t *)frame.cpu.general.ecx) = read_directory(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;
    // Syscall 2 = write
    case 2:
        *((int8_t *)frame.cpu.general.ecx) = write(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;
    // Syscall 3 = delete
    case 3:
        *((int8_t *)frame.cpu.general.ecx) = delete (*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;
    // Syscall 4 = get_keyboard_buffer
    case 4:
        get_keyboard_buffer((char *)frame.cpu.general.ebx);
        break;
    // Syscall 5 = putchar
    case 5:
        putchar(frame.cpu.general.ebx,
                frame.cpu.general.ecx);
        break;
    // Syscall 6 = puts
    case 6:
        puts(
            (char *)frame.cpu.general.ebx,
            frame.cpu.general.ecx,
            frame.cpu.general.edx);
        break;
    // Syscall 7 = activate_keyboard
    case 7:
        keyboard_state_activate();
        break;
    // Syscall 8 = clear frame
    case 8:
        framebuffer_clear();
        framebuffer_set_cursor(0, 0);
        break;
    // Syscall 11 = exec
    case 11:
        *((int8_t *)frame.cpu.general.ecx) =  process_create_user_process(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;
        break;
    case 9:
        for (int i = 0; i < 16; i++)
        {
            if (_process_list[i].metadata.pid != 0)
            {
                char *pid = int_to_string(_process_list[i].metadata.pid);
                puts("Process ID: ", 12, 0xE);
                puts(pid, strlen(pid), 0xE);
                puts("\n", 1, 0xE);

                char *state = _process_list[i].metadata.state == PROCESS_STATE_RUNNING ? "Running" : "Ready";

                puts("Process State: ", 15, 0xE);
                puts(state, strlen(state), 0xE);
                puts("\n", 1, 0xE);

                char *name = _process_list[i].metadata.name;
                puts("Process Name: ", 14, 0xE);
                puts(name, strlen(name), 0xE);
                puts("\n", 1, 0xE);
            }
        }
    case 10:
        // Syscal 10 = exit process (called when process terminated)
    }
}

void activate_timer_interrupt(void)
{
    __asm__ volatile("cli");
    // Setup how often PIT fire
    uint32_t pit_timer_counter_to_fire = PIT_TIMER_COUNTER;
    out(PIT_COMMAND_REGISTER_PIO, PIT_COMMAND_VALUE);
    out(PIT_CHANNEL_0_DATA_PIO, (uint8_t)(pit_timer_counter_to_fire & 0xFF));
    out(PIT_CHANNEL_0_DATA_PIO, (uint8_t)((pit_timer_counter_to_fire >> 8) & 0xFF));

    // Activate the interrupt
    out(PIC1_DATA, in(PIC1_DATA) & ~(1 << IRQ_TIMER));
}
