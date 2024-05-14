#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/shell/commands/commands.h"
#include "header/shell/utils/shellutils.h"

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
    char input_buffer[256];
    int16_t last_idx = -1;
    memset(input_buffer, '\0', 256);
    // Activate keyboard
    syscall(7, 0, 0, 0);
    while (true)
    {
        char *cwd = cwd_table.table[0].name;
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
                            uint32_t current_cluster_number = cwd_table.table[0].cluster_low | ((uint32_t)cwd_table.table[0].cluster_high) << 16;
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
                    ls(&cwd_table);
                    // // !!! must incorporate error-handling !!!
                    // for (int i = 2; i < 64; i++)
                    // {
                    //     if (cwd_table.table[i].user_attribute == UATTR_NOT_EMPTY)
                    //     {
                    //         shell_put(cwd_table.table[i].name, BIOS_WHITE);
                    //         shell_put(".", BIOS_WHITE);
                    //         shell_put(cwd_table.table[i].ext, BIOS_WHITE);
                    //         shell_put(" ", BIOS_WHITE);
                    //     }
                    // }
                    // shell_put("\n", BIOS_WHITE);
                }

                // COMMAND MKDIR
                else if (memcmp(command_name, "mkdir", 5) == 0)
                {
                    mkdir(args, &cwd_table);
                }

                // COMMAND CAT
                else if (memcmp(command_name, "cat", 3) == 0)
                {
                    bool found = false;
                    int i = 2;
                    char name[9];
                    char ext[4];
                    parse_file_name(args, name, ext);
                    while (!found && i < 64)
                    {
                        if (memcmp(cwd_table.table[i].name, name, 8) == 0 && memcmp(cwd_table.table[i].ext, ext, 3) == 0 && cwd_table.table[i].attribute != ATTR_SUBDIRECTORY)
                        {
                            char cat_buffer[4096];
                            uint32_t current_cluster_number = cwd_table.table[0].cluster_low | ((uint32_t)cwd_table.table[0].cluster_high) << 16;
                            struct FAT32DriverRequest cat_req = {
                                .buf = cat_buffer,
                                .parent_cluster_number = current_cluster_number,
                                .buffer_size = 4096,
                            };
                            memcpy(cat_req.name, name, 8);
                            memcpy(cat_req.ext, ext, 3);
                            int32_t cat_ret;
                            syscall(0, (uint32_t)&cat_req, (uint32_t)&cat_ret, 0);
                            shell_put(cat_buffer, BIOS_WHITE);
                            shell_put("\n", BIOS_WHITE);
                            found = true;
                        }
                        else
                        {
                            i++;
                        }
                    }
                }
                // COMMAND RM
                else if (memcmp(command_name, "rm", 2) == 0)
                {
                    char name[9];
                    char ext[4];
                    memset(name, 0, 9);
                    memset(ext, 0, 4);
                    parse_file_name(args, name, ext);
                    rm(name, ext, &cwd_table);
                }

                // RESET input_buffer UPON COMMAND EXECUTION
                last_idx = -1;
                memset(input_buffer, '\0', 256);
                command_executed = true;
            }
        }
    }

    return 0;
}