#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/shell/commands/commands.h"
#include "header/shell/utils/shellutils.h"

int main(void)
{
    struct ClusterBuffer cl[2] = {0};
    struct FAT32DriverRequest request = {
        .buf = &cl,
        .name = "shell",
        .ext = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size = CLUSTER_SIZE,
    };
    int32_t retcode;
    syscall(0, (uint32_t)&request, (uint32_t)&retcode, 0);
    // if (retcode == 0)
    //     syscall(6, (uint32_t) "owo\n", 4, 0xF);

    // Syscall 0 = read
    // Syscall 1 = read_directory
    // Syscall 2 = write
    // Syscall 3 = delete
    // Syscall 4 = get_keyboard_buffer
    // Syscall 5 = putchar
    // Syscall 6 = puts
    // Syscall 7 = activate keyword
    // Syscall 8 = getcwd
    // syscall(8, (uint32_t)&cwd, 0, 0);
    // syscall(6, (uint32_t)&cwd, 4, 0xF);
    syscall(7, 0, 0, 0);
    // struct FAT32DirectoryTable cwdb;
    // syscall(9, (uint32_t)&cwdb, 0, 0);

    char* cwd = NULL;
    syscall(8, (uint32_t)cwd, 0, 0);
    // for (int i = 0; i < 64; i++)
    // {
    //     if (cwdb.table[i].user_attribute == UATTR_NOT_EMPTY)
    //     {

    //         shell_put(cwdb.table[i].name, 0xF);
    //         shell_put(".", 0xF);
    //         shell_put(cwdb.table[i].ext, 0xF);
    //         shell_put("\n", 0xF);
    //     }
    // }

    char input_buffer[256];
    int16_t last_idx = -1;
    memset(input_buffer, 0, 256);
    while (true)
    {
        shell_put("Root@OS-IF2230", 0x2);
        shell_put(":", 0xF);
        shell_put("/", 0x9);
        shell_put(cwd, 0x9);
        shell_put("$ ", 0xF);
        bool command_executed = false;
        while (!command_executed)
        {
            // Reading each char input
            char buf;
            syscall(4, (uint32_t)&buf, 0, 0);
            if (buf)
            {
                if (memcmp(&buf, "\b", 1) == 0)
                {
                    if (last_idx >= 0)
                    {
                        last_idx--;
                        syscall(5, buf, 0xF, 0);
                    }
                }
                else
                {
                    syscall(5, buf, 0xF, 0);
                    last_idx++;
                    input_buffer[last_idx] = buf;
                }
            }
            // Calling command function after uses presses enter
            if (memcmp(&buf, "\n", 1) == 0)
            {
                // int8_t return_code;
                execute_commands(input_buffer);
                // ls();
                last_idx = -1;
                memset(input_buffer, 0, 256);
                command_executed = true;
            }
        }
    }

    return 0;
}