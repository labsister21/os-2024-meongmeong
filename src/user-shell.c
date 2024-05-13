#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/shell/commands/commands.h"
#include "header/shell/utils/shellutils.h"

#define BIOS_GREEN 0x2
#define BIOS_LIGHT_BLUE 0x9
#define BIOS_WHITE 0xF

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
    //     syscall(6, (uint32_t) "owo\n", 4, BIOS_WHITE);

    // Syscall 0 = read
    // Syscall 1 = read_directory
    // Syscall 2 = write
    // Syscall 3 = delete
    // Syscall 4 = get_keyboard_buffer
    // Syscall 5 = putchar
    // Syscall 6 = puts
    // Syscall 7 = activate keyword
    // syscall(8, (uint32_t)&cwd, 0, 0);
    // syscall(6, (uint32_t)&cwd, 4, BIOS_WHITE);
    syscall(7, 0, 0, 0);

    char cwd[11];
    memcpy(cwd, "root", 4);

    struct FAT32DirectoryTable cwd_table;
    struct FAT32DriverRequest init_req = {
        .buf = &cwd_table,
        .ext = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size = sizeof(struct FAT32DirectoryTable),
    };
    memcpy(init_req.name, "root", 4);
    int32_t ret;
    syscall(1, (uint32_t)&init_req, (uint32_t)&ret, 0);
    

    char input_buffer[256];
    int16_t last_idx = -1;
    memset(input_buffer, 0, 256);
    while (true)
    {
        shell_put("root@OS-IF2230", BIOS_GREEN);
        shell_put(":", BIOS_WHITE);
        shell_put("/", BIOS_LIGHT_BLUE);
        if (memcmp(cwd, "root", 4))
        {
            shell_put(cwd, BIOS_LIGHT_BLUE);
            shell_put("/", BIOS_LIGHT_BLUE);
        }
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
                input_buffer[last_idx] = 0;
                last_idx--;
                /* IDEALNYA MEMANGGIL execute_commands() SEPERTI DI BAWAH
                    int8_t return_code;
                    execute_commands(input_buffer);
                    ls();
                */
                char command_name[256]; 
                char args[256];
                memset(command_name, 0, 256);
                memset(args, 0, 256);

                parse_user_input(input_buffer, command_name, args);

                // COMMAND CD
                if (memcmp(command_name, "cd", 2) == 0)
                {
                    bool found = false;
                    int i = 2;
                    while (!found && i < 64)
                    {
                        if (memcmp(cwd_table.table[i].name, args, 8) == 0 && cwd_table.table[i].attribute == ATTR_SUBDIRECTORY)
                        {
                            // bisa bug, harus diperiksa
                            uint32_t current_cluster_number = cwd_table.table[0].cluster_low | ((uint32_t) cwd_table.table[0].cluster_high) << 16;
                            struct FAT32DriverRequest cd_req = {
                                .buf = &cwd_table,
                                .ext = "\0\0\0",
                                .parent_cluster_number = current_cluster_number,
                                .buffer_size = sizeof(struct FAT32DirectoryTable),
                            };
                            memcpy(cd_req.name, args, 8);
                            int32_t ret;
                            syscall(1, (uint32_t)&cd_req, (uint32_t)&ret, 0);
                            memcpy(cwd, cwd_table.table[0].name, 8);
                            found = true;
                        }
                        else
                        {
                            i++;
                        }
                    }
                }

                // COMMAND LS
                else if (memcmp(command_name, "ls", 2) == 0)
                {
                    uint32_t current_cluster_number = cwd_table.table[0].cluster_low | ((uint32_t) cwd_table.table[0].cluster_high) << 16;
                    struct FAT32DirectoryTable dirtable;
                    struct FAT32DriverRequest req = {
                        .buf = &dirtable,
                        .ext = "\0\0\0",
                        .parent_cluster_number = current_cluster_number,
                        .buffer_size = sizeof(struct FAT32DirectoryTable),
                    };
                    memcpy(req.name, cwd, 8);

                    int32_t retval;
                    syscall(1, (uint32_t)&req, (uint32_t)&retval, 0);

                    // !!! must incorporate error-handling !!!
                    for (int i = 2; i < 64; i++)
                    {
                        if (dirtable.table[i].user_attribute == UATTR_NOT_EMPTY)
                        {

                            shell_put(dirtable.table[i].name, BIOS_WHITE);
                            shell_put(".", BIOS_WHITE);
                            shell_put(dirtable.table[i].ext, BIOS_WHITE);
                            shell_put(" ", BIOS_WHITE);
                        }
                    }
                    shell_put("\n", BIOS_WHITE);
                }

                // COMMAND MKDIR
                else if (memcmp(command_name, "mkdir", 5) == 0)
                {
                    uint32_t current_cluster_number = cwd_table.table[0].cluster_low | ((uint32_t) cwd_table.table[0].cluster_high) << 16;
                    struct FAT32DriverRequest req = {
                        .buf = NULL,
                        .ext = "\0\0\0",
                        .parent_cluster_number = current_cluster_number,
                        .buffer_size = 0,
                    };
                    memcpy(req.name, args, 8);
                    syscall(2, (uint32_t)& req, (uint32_t)&retcode, 0);

                    struct FAT32DriverRequest renew_req = {
                        .buf = &cwd_table,
                        .ext = "\0\0\0",
                        .parent_cluster_number = current_cluster_number,
                        .buffer_size = sizeof(struct FAT32DirectoryTable),
                    };
                    memcpy(renew_req.name, cwd, 8);
                    int32_t renew_ret;
                    syscall(1, (uint32_t)&renew_req, (uint32_t)&renew_ret, 0);
                }

                // RESET input_buffer UPON COMMAND EXECUTION
                last_idx = -1;
                memset(input_buffer, 0, 256);
                command_executed = true;
            }
        }
    }

    return 0;
}