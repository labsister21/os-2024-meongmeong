#include "../../header/shell/commands/commands.h"

void execute_commands(char *buffer, struct DirTableStack *dts)
{
    char command_name[256];
    char args[256];
    memset(command_name, '\0', 256);
    memset(args, '\0', 256);
    size_t num_of_args = 0;

    parse_user_input(buffer, command_name, args);

    // if Args is not empty
    if (strlen(args) > 0)
    {
        num_of_args = parse_num_args(args);
    }

    // Begin casing
    if (memcmp(command_name, "cd", strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            cd(args, dts);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: cd [directory name]\n", BIOS_YELLOW);
        }
    }
    else if (memcmp(command_name, "ls", strlen(command_name)) == 0)
    {
        if (num_of_args == 0)
        {
            ls(dts);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: ls\n", BIOS_YELLOW);
        }
    }
    else if (memcmp(command_name, "mkdir", strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            mkdir(args, dts);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: mkdir [directory name]\n", BIOS_YELLOW);
        }
    }
    else if (memcmp(command_name, "cat", strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            // cat(args, dts);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: cat [file name]\n", BIOS_YELLOW);
        }
    }
    else if (memcmp(command_name, "cp", strlen(command_name)) == 0)
    {
        if (num_of_args == 2)
        {
            char output[12][128];
            strparse(args, output, " ");
            // cp(output[0],output[1], dts);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: cp [source_dir] [dest_dir]\n", BIOS_YELLOW);
        }
    }
    else if (memcmp(command_name, "rm", strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            rm(args, dts);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: rm [file/folder name] (only empty folder)\n", BIOS_YELLOW);
        }
    }
    else if (memcmp(command_name, "mv", strlen(command_name)) == 0)
    {
        if (num_of_args == 2)
        {
            char output[12][128];
            strparse(args, output, " ");
            // mv(output[0], output[1], dts);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: mv [source_dir] [dest_dir]\n", BIOS_YELLOW);
        }
    }
    else if (memcmp(command_name, "find", strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            find(args);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: find [file/folder name]\n", BIOS_YELLOW);
        }
    }
    else if (memcmp(command_name, "help", strlen(command_name)) == 0)
    {
        if (num_of_args == 0)
        {
            help();
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: help\n", BIOS_YELLOW);
        }
    }
    else if (memcmp(command_name, "clear", strlen(command_name)) == 0)
    {
        if (num_of_args == 0)
        {
            clear();
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: clear\n", BIOS_YELLOW);
        }
    }
    else
    {
        shell_put("Command not found !\n", BIOS_RED);
        help();
    }
}

void cd(char *path, struct DirTableStack *dts)
{
    // Initialize things
    struct FAT32DirectoryTable cwd_table;
    struct FAT32DriverRequest req;
    struct DirTableStack dts_copy;

    deep_copy_dirtable_stack(&dts_copy, dts);

    char paths[12][128];

    uint8_t path_num = strparse(path, paths, "/");
    // Placeholder for FAT32DriverRequest

    char name[9];
    char ext[4];

    int8_t retcode;

    // Parse Path
    for (uint8_t i = 0; i < path_num; i++)
    {
        peek(&dts_copy, &cwd_table);
        uint32_t current_cluster_number = get_cluster_number(&cwd_table);
        parse_file_name(paths[i], name, ext);
        memset(name, '\0', 9);
        memset(ext, '\0', 4);
        parse_file_name(paths[i], name, ext);
        // Kalo titik dua naik
        if (strlen(paths[i]) == 2 && memcmp(paths[i], "..", strlen(paths[i])) == 0)
        {
            if (!pop(&dts_copy))
            {
                shell_put_with_nextline("Cant go beyond root directory!", BIOS_RED);
                return;
            }
        }
        // Kalo titik satu skip
        else if (strlen(paths[i]) == 1 && memcmp(paths[i], ".", strlen(paths[i])) == 0)
        {
            continue;
        }
        // Kalo mencoba masuk ke file
        else if (memcmp(ext, "\0\0\0", 3) != 0)
        {
            shell_put_with_nextline("Invalid Path, cant go to a text file !", BIOS_RED);
            return;
        }
        else
        {
            make_request(&req, &cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, name, ext);
            retcode = sys_read_dir(&req);
            if (retcode != 0)
            {
                shell_put_with_nextline("Invalid Path one of the directory doesnt exsists!", BIOS_RED);
                return;
            }
            push(&dts_copy, &cwd_table);
        }
    }

    deep_copy_dirtable_stack(dts, &dts_copy);
}

void ls(struct DirTableStack *dts)
{
    struct FAT32DirectoryTable cwd_table;
    peek(dts, &cwd_table);
    for (int i = 2; i < 64; i++)
    {
        if (cwd_table.table[i].user_attribute == UATTR_NOT_EMPTY)
        {
            // If folder
            if (cwd_table.table[i].attribute != ATTR_SUBDIRECTORY)
            {
                shell_put(cwd_table.table[i].name, BIOS_WHITE);
                shell_put(".", BIOS_WHITE);
                shell_put(cwd_table.table[i].ext, BIOS_WHITE);
                shell_put(" ", BIOS_WHITE);
                shell_put("\n", BIOS_WHITE);
            }
            else
            {
                shell_put(cwd_table.table[i].name, BIOS_CYAN);
                shell_put("\n", BIOS_WHITE);
            }
        }
    }
}

void mkdir(char *path, struct DirTableStack *dts)
{
    // write the dir to path
    struct FAT32DirectoryTable cwd_table;
    struct FAT32DriverRequest req;
    struct DirTableStack dts_copy;
    deep_copy_dirtable_stack(&dts_copy, dts);

    int8_t retcode;
    char paths[12][128];
    // parse filepath relative to current director
    uint8_t num_path = strparse(path, paths, "/");

    char name[9];
    char ext[4];
    memset(name, 0, 9);
    memset(ext, 0, 4);

    for (uint8_t i = 0; i < num_path; i++)
    {
        peek(&dts_copy, &cwd_table);
        uint32_t parent_cluster_number = get_cluster_number(&cwd_table);
        memset(name, 0, 9);
        memset(ext, 0, 4);
        parse_file_name(paths[i], name, ext);

        // Kalo titik dua naik
        if (strlen(paths[i]) == 2 && memcmp(paths[i], "..", strlen(paths[i])) == 0)
        {
            if (!pop(&dts_copy))
            {
                shell_put("Invalid Path, Cant go beyond root dir!", BIOS_RED);
                return;
            }
        }
        // Kalo titik satu skip
        else if (strlen(paths[i]) == 1 && memcmp(paths[i], ".", strlen(paths[i])) == 0)
        {
            continue;
        }
        // Kalo dia ada naro extension
        else if (memcmp(ext, "\0\0\0", 3))
        {
            shell_put("Invalid Path, folder cant have extension!", BIOS_RED);
            return;
        }
        else
        {
            // Cek apakah ada nama path di dalam directory
            make_request(&req, &cwd_table, sizeof(struct FAT32DirectoryTable), parent_cluster_number, paths[i], "\0\0\0");
            retcode = sys_read_dir(&req);

            // Kalau ternyata directory sudah ada
            if (retcode == 0 && i == num_path - 1)
            {
                shell_put_with_nextline("Directory already exists !", BIOS_RED);
                return;
            }
            // Kalau dia ternyata ngga ada, tapi ini belum path terakhir
            else if (retcode != 0 && i != num_path - 1)
            {
                shell_put_with_nextline("Invalid Path, check you filepath!", BIOS_RED);
                return;
            }
            // Kalau ada tapi bukan path terakhir atau tidak ada tapi sudah path terakhir
            else
            {
                // kalau sudah path terakhir, maka buat directory
                if (i == num_path - 1)
                {
                    make_request(&req, NULL, 0, parent_cluster_number, name, ext);
                    retcode = sys_write(&req);
                    if (retcode == 0)
                        shell_put_with_nextline("Directory Created !", BIOS_WHITE);
                    else if (retcode == 1)
                        shell_put_with_nextline("Failed to create directory, file/folder already exsists !", BIOS_RED);
                    else if (retcode == 2)
                        shell_put_with_nextline("Failed to create directory, invalid parent cluster !", BIOS_RED);
                    else
                        shell_put_with_nextline("Failed to create directory, unknown error !", BIOS_RED);
                }
                else
                {
                    for (int i = 2; i < 64; i++)
                    {
                    }
                    push(&dts_copy, &cwd_table);
                }
            }
        }
    }
}

// void cat(char *path, struct DirTableStack *dts);
// {
//     bool found = false;
//     int i = 2;
//     while (!found && i < 64)
//     {
//         if (memcmp(cwd_table.table[i].name, name, 8) == 0 && memcmp(cwd_table.table[i].ext, ext, 3) == 0 && cwd_table.table[i].attribute != ATTR_SUBDIRECTORY)
//         {
//             char cat_buffer[4096];
//             uint32_t current_cluster_number = cwd_table.table[0].cluster_low | ((uint32_t)cwd_table.table[0].cluster_high) << 16;
//             struct FAT32DriverRequest cat_req = {
//                 .buf = cat_buffer,
//                 .parent_cluster_number = current_cluster_number,
//                 .buffer_size = 4096,
//             };
//             memcpy(cat_req.name, name, 8);
//             memcpy(cat_req.ext, ext, 3);
//             int32_t cat_ret;
//             syscall(0, (uint32_t)&cat_req, (uint32_t)&cat_ret, 0);
//             shell_put(cat_buffer, BIOS_WHITE);
//             shell_put("\n", BIOS_WHITE);
//             found = true;
//         }
//         else
//         {
//             i++;
//         }
//     }
// }

// void cp(char *filename)
// {
// }

void rm(char *path, struct DirTableStack *dts)
{
    // Initialize things
    struct FAT32DirectoryTable cwd_table;
    struct FAT32DriverRequest req;
    struct DirTableStack dts_copy;

    deep_copy_dirtable_stack(&dts_copy, dts);

    char paths[12][128];

    uint8_t path_num = strparse(path, paths, "/");
    // Placeholder for FAT32DriverRequest

    char name[9];
    char ext[4];

    int8_t retcode;

    // Parse Path
    for (uint8_t i = 0; i < path_num; i++)
    {
        peek(&dts_copy, &cwd_table);
        uint32_t current_cluster_number = get_cluster_number(&cwd_table);
        memset(name, '\0', 9);
        memset(ext, '\0', 4);
        parse_file_name(paths[i], name, ext);
        // Kalo titik dua naik
        if (strlen(paths[i]) == 2 && memcmp(paths[i], "..", strlen(paths[i])) == 0)
        {
            pop(&dts_copy);
        }
        // Kalo titik satu skip
        else if (strlen(paths[i]) == 1 && memcmp(paths[i], ".", strlen(paths[i])) == 0)
        {
            continue;
        }

        else if (memcmp(ext, "\0\0\0", 3) != 0)
        {
            // Kalo dia file, tapi bukan end of dir
            if (i != path_num - 1)
            {
                shell_put_with_nextline("Invalid Path !", BIOS_RED);
                return;
            }
            else
            {
                struct FAT32DriverRequest req;
                make_request(&req, NULL, sizeof(struct FAT32DirectoryTable), current_cluster_number, name, ext);
                retcode = sys_delete(&req);
            }
        }
        // Kalo dia directory, maka push ke curr directory
        else
        {
            // Kalau belum terakhir
            if (i != path_num - 1)
            {
                struct FAT32DriverRequest req;
                make_request(&req, &cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, name, "\0\0\0");
                retcode = sys_read_dir(&req);
                if (retcode != 0)
                {
                    shell_put_with_nextline("Invalid Path !", BIOS_RED);
                    return;
                }
                push(&dts_copy, &cwd_table);
            }
            // Kalau udah terakhir langsung delete
            else
            {
                make_request(&req, NULL, sizeof(struct FAT32DirectoryTable), current_cluster_number, name, ext);
                retcode = sys_delete(&req);
            }
        }
    }

    if (retcode == 0)
    {
        shell_put_with_nextline("File deleted !", BIOS_GREEN);
    }
    else if (retcode == 2)
    {
        shell_put_with_nextline("Folder is not empty!", BIOS_RED);
    }
    else
    {
        shell_put_with_nextline("Failed to delete file :(", BIOS_RED);
    }
}

// void mv(char *filename, char *dirname)
// {
// }

void find(char *filename)
{
    // Make a stack with root as its base
    struct DirTableStack dts;
    struct FAT32DirectoryTable root_table;
    struct FAT32DriverRequest req;

    initialize_stack(&dts);

    char name[9];
    char ext[4];
    memset(name, 0, 9);
    memset(ext, 0, 4);
    parse_file_name(filename, name, ext);

    make_request(&req, &root_table, sizeof(struct FAT32DirectoryTable), ROOT_CLUSTER_NUMBER, "root\0\0\0\0", "\0\0\0");
    int8_t retcode = sys_read_dir(&req);

    if (retcode != 0)
    {
        shell_put_with_nextline("Failed to read root directory !", BIOS_RED);
        return;
    }
    else
    {
        push(&dts, &root_table);
        find_helper(name, ext, &dts);
    }

    // if find root
    if (memcmp(name, "root", 4) == 0 && memcmp(ext, "\0\0\0", 3) == 0)
    {
        shell_put_with_nextline("root\n", BIOS_WHITE);
    }
}

void find_helper(char *name, char *ext, struct DirTableStack *dts)
{
    // Initialize things
    struct FAT32DirectoryTable cwd_table;
    struct FAT32DriverRequest req;

    struct DirTableStack dts_copy;
    deep_copy_dirtable_stack(&dts_copy, dts);
    peek(&dts_copy, &cwd_table);

    for (int i = 2; i < 64; i++)
    { // Kalau dia ada isinya
        if (cwd_table.table[i].user_attribute == UATTR_NOT_EMPTY)
        {
            if (memcmp(cwd_table.table[i].name, name, 8) == 0 && memcmp(cwd_table.table[i].ext, ext, 3) == 0)
            {

                char namebuffer[13];
                memset(namebuffer, 0, 13);
                strcpy(namebuffer, cwd_table.table[i].name);
                // Concat with extension if there is any
                if (memcmp(cwd_table.table[i].ext, "\0\0\0", 3) != 0)
                {
                    strcat(namebuffer, ".");
                    strcat(namebuffer, cwd_table.table[i].ext);
                }

                get_full_path(&dts_copy);
                shell_put(namebuffer, BIOS_WHITE);
                shell_put("\n", BIOS_WHITE);
            }
            if (cwd_table.table[i].attribute == ATTR_SUBDIRECTORY)
            {
                uint32_t current_cluster_number = get_cluster_number(&cwd_table);
                struct FAT32DriverRequest req;
                make_request(&req, &cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, cwd_table.table[i].name, cwd_table.table[i].ext);
                int8_t retcode = sys_read_dir(&req);
                if (retcode != 0)
                {
                    shell_put_with_nextline("Failed to read directory !", BIOS_RED);
                    return;
                }
                push(&dts_copy, &cwd_table);
                find_helper(name, ext, &dts_copy);
            }
        }
    }
}

void help()
{
    shell_put("List Commands: \n", BIOS_WHITE);
    shell_put("1. cd : Mengganti current working directory\n", BIOS_WHITE);
    shell_put("     penggunaan: cd [nama_directory]\n", BIOS_YELLOW);
    shell_put("2. ls : Menuliskan isi current working directory\n", BIOS_WHITE);
    shell_put("     penggunaan: ls\n", BIOS_YELLOW);
    shell_put("3. mkdir : Membuat sebuah folder kosong baru\n", BIOS_WHITE);
    shell_put("     penggunaan: mkdir [nama_directory]\n", BIOS_YELLOW);
    shell_put("4. cat : Menuliskan sebuah file sebagai text file ke layar \n", BIOS_WHITE);
    shell_put("     penggunaan: cat [nama_file]\n", BIOS_YELLOW);
    shell_put("5. cp : Mengcopy suatu file\n", BIOS_WHITE);
    shell_put("     penggunaan: cp [source_dir] [dest_dir]\n", BIOS_YELLOW);
    shell_put("6. rm : Menghapus suatu file\n", BIOS_WHITE);
    shell_put("     penggunaan: rm [nama_file]\n", BIOS_YELLOW);
    shell_put("7. mv : Memindah dan merename lokasi file/folder\n", BIOS_WHITE);
    shell_put("     penggunaan: mv [source_dir] [dest_dir]\n", BIOS_YELLOW);
    shell_put("8. find : Mencari file/folder dengan nama yang sama diseluruh file system\n", BIOS_WHITE);
    shell_put("     penggunaan: find [nama_directory]\n", BIOS_YELLOW);
}

void clear()
{
    sys_clear();
}
