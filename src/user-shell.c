#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/shell/commands/commands.h"
#include "header/shell/utils/shellutils.h"
#include "header/shell/datastructure/dirtablestack.h"
#define BIOS_GREEN 0x2
#define BIOS_LIGHT_BLUE 0x9
#define BIOS_WHITE 0xF

int main(void)
{
    // Init cwdtable
    struct FAT32DirectoryTable cwd_table;
    struct FAT32DriverRequest init_req;
    make_request(&init_req, &cwd_table, sizeof(struct FAT32DirectoryTable), ROOT_CLUSTER_NUMBER, "root\0\0\0\0", "\0\0\0");
    int32_t ret;
    syscall(1, (uint32_t)&init_req, (uint32_t)&ret, 0);

    // Init stack
    struct DirTableStack dts;
    dts.idx_top = -1;
    push(&dts, &cwd_table);

    // Init buffer stuff
    char input_buffer[256];
    int16_t last_idx = -1;
    memset(input_buffer, '\0', 256);

    // Activate keyboard
    syscall(7, 0, 0, 0);
    while (true)
    {
        shell_put("root@OS-IF2230", BIOS_GREEN);
        shell_put(":", BIOS_WHITE);
        shell_put("/", BIOS_LIGHT_BLUE);
        get_full_path(&dts);
        shell_put("$ ", BIOS_WHITE);
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
                        input_buffer[last_idx] = '\0';
                        last_idx--;
                        syscall(5, buf, BIOS_WHITE, 0);
                    }
                }
                else
                {
                    syscall(5, buf, BIOS_WHITE, 0);
                    last_idx++;
                    input_buffer[last_idx] = buf;
                }
            }
            // Calling command function after uses presses enter
            if (memcmp(&buf, "\n", 1) == 0)
            {
                input_buffer[last_idx] = '\0';
                last_idx--;
                /* IDEALNYA MEMANGGIL execute_commands() SEPERTI DI BAWAH
                    int8_t return_code;
                    execute_commands(input_buffer);
                    ls();
                */
                execute_commands(&input_buffer,&dts);

                // RESET input_buffer UPON COMMAND EXECUTION
                last_idx = -1;
                memset(input_buffer, '\0', 256);
                command_executed = true;
            }
        }
    }

    return 0;
}