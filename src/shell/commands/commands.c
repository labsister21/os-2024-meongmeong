#include "../../header/shell/commands/commands.h"

void execute_commands(char *buffer)
{
    char command_name[256];
    char args[256];
    memset(command_name, 0, 256);
    memset(args, 0, 256);

    parse_user_input(buffer, command_name, args);

    // return 1;
    // else
    // {

    //     shell_put()
    // }
}

// void cd(char *dir)
// {
// }

void ls(struct FAT32DirectoryTable *cwd_table)
{
    for (int i = 2; i < 64; i++)
    {
        if (cwd_table->table[i].user_attribute == UATTR_NOT_EMPTY)
        {
            shell_put(cwd_table->table[i].name, BIOS_WHITE);
            shell_put(".", BIOS_WHITE);
            shell_put(cwd_table->table[i].ext, BIOS_WHITE);
            shell_put(" ", BIOS_WHITE);
        }
    }
    shell_put("\n", BIOS_WHITE);
}

void mkdir(char *dirname, struct FAT32DirectoryTable *cwd_table)
{
    // Cek apakah direktori sudah ada
    // Jika sudah ada, return
    // Jika belum, buat direktori
    // Jika berhasil, ubah cwd_table
    int32_t retcode;
    uint32_t current_cluster_number = cwd_table->table[0].cluster_low | ((uint32_t)cwd_table->table[0].cluster_high) << 16;
    struct FAT32DriverRequest req = {
        .buf = NULL,
        .ext = "\0\0\0",
        .parent_cluster_number = current_cluster_number,
        .buffer_size = 0,
    };
    memcpy(req.name, dirname, 8);
    syscall(2, (uint32_t)&req, (uint32_t)&retcode, 0);

    if (retcode == 0)
    {
        shell_put("Directory created !\n", BIOS_GREEN);
    }
    else
    {
        shell_put("Failed to create directory :( \n", BIOS_RED);
    }

    update_cwd_table(cwd_table);
}

void cat(char *name, char *ext, struct FAT32DirectoryTable cwd_table)
{
    bool found = false;
    int i = 2;
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

// void cp(char *filename)
// {
// }

void rm(char *name, char *ext, struct FAT32DirectoryTable *cwd_table)
{
    // Placeholder for FAT32DriverRequest
    struct FAT32DriverRequest req;
    uint32_t parent_cluster_number = cwd_table->table[0].cluster_low | ((uint32_t)cwd_table->table[0].cluster_high) << 16;
    make_request(&req, NULL, sizeof(struct FAT32DirectoryTable), parent_cluster_number, name, ext);
    int8_t retcode;
    syscall(3, (uint32_t)&req, (uint32_t)&retcode, 0);

    if (retcode == 0)
    {
        shell_put("File deleted !\n", BIOS_GREEN);
    }
    else
    {
        shell_put("Failed to delete file :( \n", BIOS_RED);
    }

    update_cwd_table(cwd_table);
}

// void mv(char *filename, char *dirname)
// {
// }

// void find(char *filename)
// {
// }