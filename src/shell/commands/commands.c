#include "../../header/shell/commands/commands.h"

void execute_commands(char *buffer, struct DirTableStack *dts)
{
    char command_name[256];
    char args[256];
    memset(command_name, '\0', 256);
    memset(args, '\0', 256);
    size_t num_of_args;

    parse_user_input(buffer, command_name, args);
    if (strlen(args) > 0)
    {size_t num_of_args = parse_num_args(args);}
    else {
        num_of_args = 0;
    }


    if (memcmp(command_name, "cd", 
    strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            // cd(args,dts);
        }
    }
    else if (memcmp(command_name, "ls", strlen(command_name)) == 0)
    {
        if (num_of_args == 0)
        {
            ls(dts);
        }
    }
    else if (memcmp(command_name, "mkdir", strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            mkdir(args,dts);
        }
    }
    else if (memcmp(command_name, "cat", strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            // cat(args, dts);
        }
    }
    else if (memcmp(command_name, "cp", strlen(command_name)) == 0)
    {
        if (num_of_args == 2)
        {
            char* output[2];
            strparse(args,output," ");
            // cp(output[0],output[1], dts);
        }
    }
    else if (memcmp(command_name, "rm", strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            rm(args, dts);
        }
    }
    else if (memcmp(command_name, "mv", strlen(command_name)) == 0)
    {
        if (num_of_args == 2)
        {   
            char* output[2];
            strparse(args,output," "); 
            // mv(output[0],output[1],dts);
        }
    }
    else if (memcmp(command_name, "find", strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            find(args);
        }
    }
    else if (memcmp(command_name, "help", strlen(command_name)) == 0)
    {
        if (num_of_args == 0)
        {    
             help();
        } else 
        {
            shell_put("Number of arguments in valid\n", BIOS_RED);
        }
    }
    else if (memcmp(command_name, "clear", strlen(command_name)) == 0)
    {
        if (num_of_args == 0)
        {
            clear();
        } else 
        {
            shell_put("Number of arguments in valid\n", BIOS_RED);
        }
    }
    else
    {
        shell_put("Command not found !\n", BIOS_RED);
    }

}

// void cd(char *path, struct DirTableStack *dts)
// {

//     // Initialize things
//     char paths[12][128];
//     uint8_t path_num = strparse(path, paths, "/");
//     char name[9];
//     char ext[4];

//     // iterate through all inputs
//     for (uint8_t i = 0; i < path_num; i++)
//     {
//         memset(name, '\0', 9);
//         memset(ext, '\0', 4);
//         parse_file_name(paths[i], name, ext);
//         if (memcmp(paths[i], "..", strlen(paths[i])) == 0)
//         {
//             pop(dts);
//         }
//         else if (memcmp(paths[i], ".", strlen(paths[i])) == 0)
//         {
//             continue;
//         }
//         else
//         {
//             uint32_t current_cluster_number = cwd_table.table[0].cluster_low | ((uint32_t)cwd_table.table[0].cluster_high) << 16;
//             struct FAT32DriverRequest req;
//             make_request(&req, &cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, paths[i], "\0\0\0");
//             int32_t retcode = sys_read_dir(&req);
//             if (retcode != 0)
//             {
//                 return retcode;
//             }
//             push(&dts_copy, &cwd_table);
//         }
//     }
// }

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
        if (memcmp(paths[i], "..", strlen(paths[i])) == 0)
        {
            if (!pop(&dts_copy))
            {
                shell_put("Invalid Path, Cant go beyond root dir!", BIOS_RED);
                return;
            }
        }
        // Kalo titik satu skip
        else if (memcmp(paths[i], ".", strlen(paths[i])) == 0)
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

void cp(char *path, char *filename, struct DirTableStack *dts)
{
    // Initialize things
    struct FAT32DirectoryTable cwd_table;
    struct FAT32DriverRequest req;
    peek(dts, &cwd_table);

    // Get the file size of the desired file to copy
    uint32_t filesize;
    int8_t retval = get_file_size(&cwd_table, filename, &filesize);

    // If filename does not exist in parent directory
    if (retval == -1)
    {
        shell_put("File not found!", BIOS_RED);
        return;
    }

    // Read from bin then write to the desired path
    char buffer[filesize];
    uint32_t parent_cluster_number = get_cluster_number(&cwd_table);

    // Read from bin
    char file_name[9];
    char file_ext[4];
    memset(file_name, '\0', 9);
    memset(file_ext, '\0', 4);
    parse_file_name(filename, file_name, file_ext);
    make_request(&req, buffer, filesize, parent_cluster_number, file_name, file_ext);
    retval = sys_read(&req);

    if (retval != 0)
    {
        shell_put("Unexcpected error occurs", BIOS_RED);
        return;
    }

    struct DirTableStack dts_copy;
    deep_copy_dirtable_stack(&dts_copy, dts);

    // Parsing path to write the copy
    char paths[12][128];
    uint8_t path_num = strparse(path, paths, "/");

    char name[9];
    char ext[4];
    for (uint8_t i = 0; i < path_num; i++)
    {
        uint32_t current_cluster_number = get_cluster_number(&cwd_table);
        memset(name, '\0', 9);
        memset(ext, '\0', 4);
        parse_file_name(paths[i], name, ext);
        // Kalo titik dua naik
        if (memcmp(paths[i], "..", strlen(paths[i])) == 0)
        {
            pop(&dts_copy);
        }
        // Kalo titik satu skip
        else if (memcmp(paths[i], ".", strlen(paths[i])) == 0)
        {
            continue;
        }
        else if (memcmp(ext, "\0\0\0", 3) != 0)
        {
            // Kalo dia file, tapi bukan end of dir
            if (i != path_num - 1)
            {
                shell_put("Invalid Path!", BIOS_RED);
                return;
            }
            else
            {
                peek(&dts_copy, &cwd_table);
                parent_cluster_number = get_cluster_number(&cwd_table);
                make_request(&req, buffer, filesize, parent_cluster_number, name, ext);
                retval = sys_write(&req);
                if (retval == 1)
                {
                    shell_put("File with the same name already exist!\n", BIOS_RED);
                    return;
                }
                else if (retval == -1)
                {
                    shell_put("Unexcpected error occurs\n", BIOS_RED);
                    return;
                }
                return;
            }
        }
        // Kalo dia directory, maka push ke curr directory
        else
        {
            make_request(&req, &cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, name, ext);
            int8_t ret = sys_read_dir(&req);
            if (ret != 0)
            {
                shell_put("Invalid path!\n", BIOS_RED);
                return;
            }
            push(&dts_copy, &cwd_table);
            if (i == path_num - 1)
            {
                parent_cluster_number = get_cluster_number(&cwd_table);
                make_request(&req, buffer, filesize, parent_cluster_number, file_name, file_ext);
                int8_t retcode = sys_write(&req);
            }
        }
    }
}

void rm(char *path, struct DirTableStack *dts)
{
    // Initialize things
    struct FAT32DirectoryTable cwd_table;
    struct FAT32DriverRequest req;
    struct DirTableStack dts_copy;

    deep_copy_dirtable_stack(&dts_copy, dts);
    peek(&dts_copy, &cwd_table);

    char paths[12][128];

    uint8_t path_num = strparse(path, paths, "/");
    // Placeholder for FAT32DriverRequest

    char name[9];
    char ext[4];

    int8_t retcode;

    // Parse Path
    for (uint8_t i = 0; i < path_num; i++)
    {
        uint32_t current_cluster_number = get_cluster_number(&cwd_table);
        memset(name, '\0', 9);
        memset(ext, '\0', 4);
        parse_file_name(paths[i], name, ext);
        // Kalo titik dua naik
        if (memcmp(paths[i], "..", strlen(paths[i])) == 0)
        {
            pop(&dts_copy);
        }
        // Kalo titik satu skip
        else if (memcmp(paths[i], ".", strlen(paths[i])) == 0)
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
                char buffer[1];
                make_request(&req, buffer, sizeof(struct FAT32DirectoryTable), current_cluster_number, name, ext);
                int8_t retcode = sys_read(&req);
                if (retcode != 0)
                {
                    shell_put_with_nextline("Invalid Filename!", BIOS_RED);
                }
                break;
            }
        }
        // Kalo dia directory, maka push ke curr directory
        else
        {
            struct FAT32DriverRequest req;
            make_request(&req, &cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, paths[i], "\0\0\0");
            int8_t retcode = sys_read_dir(&req);
            if (retcode != 0)
            {
                shell_put_with_nextline("Invalid Path !", BIOS_RED);
                return;
            }
            if (i != path_num - 1)
            {
                push(&dts_copy, &cwd_table);
            }
            else
            {
                peek(&dts_copy, &cwd_table);
                uint32_t parent_cluster_number = get_cluster_number(&cwd_table);
                make_request(&req, NULL, sizeof(struct FAT32DirectoryTable), parent_cluster_number, name, ext);
                int8_t retcode = sys_delete(&req);
            }
        }
    }

    if (retcode == 0)
    {
        shell_put_with_nextline("File deleted !\n", BIOS_GREEN);
    }
    else if (retcode == 2)
    {
        shell_put_with_nextline("Folder is not empty!", BIOS_RED);
    }
    else
    {
        shell_put_with_nextline("Failed to delete file :( \n", BIOS_RED);
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
    shell_put("1. cd : Mengganti current working directory\n",BIOS_WHITE);
    shell_put("     penggunaan: cd [nama_directory]\n", BIOS_YELLOW);
    shell_put("2. ls : Menuliskan isi current working directory\n",BIOS_WHITE);
    shell_put("     penggunaan: ls\n", BIOS_YELLOW);
    shell_put("3. mkdir : Membuat sebuah folder kosong baru\n",BIOS_WHITE);
    shell_put("     penggunaan: mkdir [nama_directory]\n", BIOS_YELLOW);
    shell_put("4. cat : Menuliskan sebuah file sebagai text file ke layar \n",BIOS_WHITE);
    shell_put("     penggunaan: cat [nama_file]\n", BIOS_YELLOW);
    shell_put("5. cp : Mengcopy suatu file\n",BIOS_WHITE);
    shell_put("     penggunaan: cp [source_dir] [dest_dir]\n", BIOS_YELLOW);
    shell_put("6. rm : Menghapus suatu file\n",BIOS_WHITE);
    shell_put("     penggunaan: rm [nama_file]\n", BIOS_YELLOW);
    shell_put("7. mv : Memindah dan merename lokasi file/folder\n",BIOS_WHITE);
    shell_put("     penggunaan: mv [source_dir] [dest_dir]\n", BIOS_YELLOW);
    shell_put("8. find : Mencari file/folder dengan nama yang sama diseluruh file system\n",BIOS_WHITE);
    shell_put("     penggunaan: find [nama_directory]\n", BIOS_YELLOW);
    
}

void clear()
{
    sys_clear();
}

