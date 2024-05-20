#include "../../header/shell/commands/commands.h"

void execute_commands(char *buffer, struct DirTableStack *dts)
{
    char command_name[256];
    char args[256];
    memset(command_name, '\0', 256);
    memset(args, '\0', 256);
    size_t num_of_args = 0;

    char output[12][128];

    parse_user_input(buffer, command_name, args);
    // if Args is not empty
    if (strlen(args) > 0)
    {
        num_of_args = strparse(args, output, " ");
    }

    // Begin casing
    if (strlen(command_name) == 2 && memcmp(command_name, "cd", strlen(command_name)) == 0)
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
    else if (strlen(command_name) == 2 && memcmp(command_name, "ls", strlen(command_name)) == 0)
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
    else if (strlen(command_name) == 5 && memcmp(command_name, "mkdir", strlen(command_name)) == 0)
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
    else if (strlen(command_name) == 3 && memcmp(command_name, "cat", strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            cat(args, dts);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: cat [file name]\n", BIOS_YELLOW);
        }
    }
    else if (strlen(command_name) == 2 && memcmp(command_name, "cp", strlen(command_name)) == 0)
    {
        if (num_of_args == 2)
        {
            cp(output[0], output[1], dts);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: cp [source_dir] [dest_dir]\n", BIOS_YELLOW);
        }
    }
    else if (strlen(command_name) == 2 && memcmp(command_name, "rm", strlen(command_name)) == 0)
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
    else if (strlen(command_name) == 2 && memcmp(command_name, "mv", strlen(command_name)) == 0)
    {
        if (num_of_args == 2)
        {
            mv(output[0], output[1], dts);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: mv [source_dir] [dest_dir]\n", BIOS_YELLOW);
        }
    }
    else if (strlen(command_name) == 4 && memcmp(command_name, "find", strlen(command_name)) == 0)
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
    else if (strlen(command_name) == 4 && memcmp(command_name, "help", strlen(command_name)) == 0)
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
    else if (strlen(command_name) == 5 && memcmp(command_name, "clear", strlen(command_name)) == 0)
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
    else if (strlen(command_name) == 2 && memcmp(command_name, "ps", strlen(command_name)) == 0)
    {
        if (num_of_args == 0)
        {
            ps();
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: ps\n", BIOS_YELLOW);
        }
    }
    else if (strlen(command_name) == 4 && memcmp(command_name, "exec", strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            exec(args, dts);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: exec [file name]\n", BIOS_YELLOW);
        }
    }
    else if (strlen(command_name) == 4 && memcmp(command_name, "kill", strlen(command_name)) == 0)
    {
        if (num_of_args == 1)
        {
            kill(args);
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: kill [pid]\n", BIOS_YELLOW);
        }
    }
    else if (strlen(command_name) == 5 && memcmp(command_name, "clock", strlen(command_name)) == 0)
    {
        if (num_of_args == 0)
        {
            clock();
        }
        else
        {
            shell_put("Number of arguments invalid\n", BIOS_RED);
            shell_put("Usage: clock\n", BIOS_YELLOW);
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

    // deep copy to make sure the change dts is
    deep_copy_dirtable_stack(dts, &dts_copy);
    return;
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

                if (memcmp(cwd_table.table[i].ext, "\0\0\0", 3) != 0)
                {
                    shell_put(".", BIOS_WHITE);
                    shell_put(cwd_table.table[i].ext, BIOS_WHITE);
                    shell_put(" ", BIOS_WHITE);
                }
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

void cat(char *path, struct DirTableStack *dts)
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
                uint32_t filesize;
                int8_t retval = get_file_size(&cwd_table, paths[i], &filesize);

                // If filename does not exist in parent directory
                if (retval == -1)
                {
                    shell_put("File not found!", BIOS_RED);
                    return;
                }

                char buffer[filesize];
                uint32_t parent_cluster_number = get_cluster_number(&cwd_table);

                // Read from bin
                make_request(&req, buffer, filesize, parent_cluster_number, name, ext);
                retcode = sys_read(&req);
                if (retcode == 0)
                {
                    shell_put_with_nextline(buffer, BIOS_LIGHT_MAGENTA);
                }
            }
        }
        // Kalo dia directory, maka push ke curr directory
        else
        {
            // Kalau belum terakhir
            if (i != path_num - 1)
            {
                make_request(&req, &cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, name, "\0\0\0");
                retcode = sys_read_dir(&req);
                if (retcode != 0)
                {
                    shell_put_with_nextline("Invalid Path !", BIOS_RED);
                    return;
                }
                push(&dts_copy, &cwd_table);
            }
            // Kalau udah terakhir langsung read
            else
            {
                uint32_t filesize;
                int8_t retval = get_file_size(&cwd_table, paths[i], &filesize);

                // If filename does not exist in parent directory
                if (retval == -1)
                {
                    shell_put_with_nextline("File not found!", BIOS_RED);
                    return;
                }

                char buffer[filesize];
                uint32_t parent_cluster_number = get_cluster_number(&cwd_table);

                // Read from bin
                make_request(&req, buffer, filesize, parent_cluster_number, name, ext);
                retcode = sys_read(&req);
                if (retcode == 0)
                {
                    shell_put_with_nextline(buffer, BIOS_LIGHT_MAGENTA);
                }
            }
        }
    }

    if (retcode == 2)
    {
        shell_put_with_nextline("Buffersize is not enough", BIOS_RED);
    }
    else if (retcode == 1)
    {
        shell_put_with_nextline("What you are trying to read is not a file!", BIOS_RED);
    }
    else if (retcode == 3)
    {
        shell_put_with_nextline("File not found!", BIOS_RED);
    }
    else if (retcode == -1)
    {
        shell_put_with_nextline("Unexcpected error occurs", BIOS_RED);
    }
}

bool cp(char *src_path, char *dest_path, struct DirTableStack *dts)
{
    // Initialization
    struct FAT32DirectoryTable cwd_table_src;
    struct FAT32DriverRequest src_req;
    struct DirTableStack dts_copy_req;

    deep_copy_dirtable_stack(&dts_copy_req, dts);

    uint32_t src_filesize;
    uint8_t src_attribute;

    // Read from bin
    char src_name[9];
    char src_ext[4];
    char src_paths[12][128];
    uint8_t path_num = strparse(src_path, src_paths, "/");

    int8_t retcode;
    uint32_t parent_cluster_number;
    bool cp_file;

    for (uint8_t i = 0; i < path_num; i++)
    {
        peek(&dts_copy_req, &cwd_table_src);
        parent_cluster_number = get_cluster_number(&cwd_table_src);
        memset(src_name, '\0', 9);
        memset(src_ext, '\0', 4);
        parse_file_name(src_path, src_name, src_ext);

        if (strlen(src_paths[i]) == 2 && memcmp(src_paths[i], "..", strlen(src_paths[i])) == 0)
        {
            pop(&dts_copy_req);
        }
        else if (strlen(src_paths[i]) == 1 && memcmp(src_paths[i], ".", strlen(src_paths[i])) == 0)
        {
        }
        else
        {
            get_size_and_attribute(&cwd_table_src, src_paths[i], &src_filesize, &src_attribute);
            if (src_attribute != ATTR_SUBDIRECTORY)
            {
                if (i != path_num - 1)
                {
                    shell_put("Invalid Path!\n", BIOS_RED);
                    return false;
                }
                else
                {
                    cp_file = true;
                }
            }
            // Jika folder
            else
            {
                if (i != path_num - 1)
                {
                    push(&dts_copy_req, &cwd_table_src);
                }
                else
                {
                    cp_file = false;
                }
            }
        }

    }

    char buffer[src_filesize];

    if (cp_file)
    {
        get_size(&cwd_table_src, src_name, &src_filesize);
        make_request(&src_req, buffer, src_filesize, parent_cluster_number, src_name, src_ext);
        sys_read(&src_req);
    }
    else
    {
        make_request(&src_req, &cwd_table_src, sizeof(struct FAT32DirectoryTable), parent_cluster_number, src_name, src_ext);
        sys_read_dir(&src_req);
    }

    // Read from bin
    char dest_name[9];
    char dest_ext[4];
    char dest_paths[12][128];
    path_num = strparse(dest_path, dest_paths, "/");

    struct FAT32DirectoryTable cwd_table_dest;
    struct FAT32DriverRequest dest_req;
    struct DirTableStack dts_copy_dest;

    uint32_t dest_filesize;
    uint8_t dest_attribute;

    for (uint8_t i = 0; i < path_num; i++)
    {
        peek(&dts_copy_dest, &cwd_table_dest);
        parent_cluster_number = get_cluster_number(&cwd_table_dest);
        memset(dest_name, '\0', 9);
        memset(dest_ext, '\0', 4);
        parse_file_name(dest_paths[i], dest_name, dest_ext);
        if (strlen(dest_paths[i]) == 2 && memcmp(dest_paths[i], "..", strlen(dest_paths[i])) == 0)
        {
            pop(&dts_copy_req);
        }
        else if (strlen(dest_paths[i]) == 1 && memcmp(dest_paths[i], ".", strlen(dest_paths[i])) == 0)
        {
        }
        else
        {
            get_size_and_attribute(&cwd_table_dest, dest_paths[i], &dest_filesize, &dest_attribute);
            if (dest_attribute != ATTR_SUBDIRECTORY)
            {
                if (i != path_num - 1)
                {
                    shell_put("Invalid Path!\n", BIOS_RED);
                    return false;
                }
                else
                {
                    // Copy file ke dalam file baru
                    if (cp_file)
                    {
                        make_request(&dest_req, buffer, src_filesize, parent_cluster_number, dest_name, dest_ext);
                        sys_write(&dest_req);
                        return true;
                    }
                    else
                    {
                        shell_put("Cannot move from folder to file!\n", BIOS_RED);
                        return false;
                    }
                }
            }
            // Jika folder
            else
            {
                push(&dts_copy_req, &cwd_table_dest);
                if (i == path_num - 1)
                {
                    parent_cluster_number = get_cluster_number(&cwd_table_dest);
                    if (cp_file)
                    {
                        make_request(&dest_req, buffer, src_filesize, parent_cluster_number, dest_name, dest_ext);
                        sys_write(&dest_req);
                    }
                    else
                    {
                            retcode = cp_helper(&cwd_table_src, parent_cluster_number, &cwd_table_dest);
                    }
                }
            }   
        }
    }
}

// Special for folder
int8_t cp_helper(struct FAT32DirectoryTable * want_to_copy, uint32_t parent_cluster_number)
{
    struct FAT32DirectoryTable new;
    int8_t retcode;
    // Make the folder in destination
    struct FAT32DriverRequest req;
    make_request(&req, NULL, 0, parent_cluster_number, want_to_copy->table[0].name, "\0\0\0");
    retcode = sys_write(&req);

    if (retcode != 0 ){
        shell_put("Unexpected error when copying directory!\n", BIOS_RED);
    }

    make_request(&req, &new, sizeof(struct FAT32DirectoryTable), parent_cluster_number,want_to_copy->table[0].name, "\0\0\0");

    retcode = sys_read_dir(&req);
    if (retcode != 0 ){
        shell_put("Unexpected error when copying directory!\n", BIOS_RED);
    }

    uint32_t new_cluster_number = (uint32_t)new.table[0].cluster_high << 16 | (uint32_t)new.table[0].cluster_low;
    for (int i = 2; i< 64 ; i ++)

    { 
        if (new.table[i].user_attribute != UATTR_NOT_EMPTY){
            if(new.table[i].attribute == ATTR_SUBDIRECTORY)
            }
    else
    {

    }}
}

// bool cp(char *src_path, char *dest_path, struct DirTableStack *dts)
// {
//     // Initialize things
//     struct FAT32DirectoryTable cwd_table;
//     struct FAT32DriverRequest req;
//     struct DirTableStack dts_copy;

//     deep_copy_dirtable_stack(&dts_copy, dts);
//     peek(&dts_copy, &cwd_table);

//     // Get the file size of the desired file to copy
//     uint32_t filesize;

//     // Read from bin
//     char src_name[9];
//     char src_ext[4];
//     char paths[12][128];
//     uint8_t path_num = strparse(src_path, paths, "/");
//     for (uint8_t i = 0; i < path_num; i++)
//     {
//         uint32_t current_cluster_number = get_cluster_number(&cwd_table);
//         memset(src_name, '\0', 9);
//         memset(src_ext, '\0', 4);
//         parse_file_name(paths[i], src_name, src_ext);
//         // Kalo titik dua naik
//         if (memcmp(paths[i], "..", strlen(paths[i])) == 0)
//         {
//             pop(&dts_copy);
//             if (i == path_num - 1)
//             {
//                 peek(&dts_copy, &cwd_table);
//                 int8_t retval = get_file_size(&cwd_table, paths[i], &filesize);

//                 if (retval == -1)
//                 {
//                     shell_put("File not found!\n", BIOS_RED);
//                     return false;
//                 }

//                 uint32_t parent_cluster_number = get_cluster_number(&cwd_table);
//                 char buffer[filesize];
//                 make_request(&req, buffer, filesize, parent_cluster_number, src_name, src_ext);
//                 int8_t retcode = sys_read(&req);
//                 if (retcode != 0)
//                 {
//                     return false;
//                 }
//                 return true;
//             }
//         }
//         // Kalo titik satu skip
//         else if (memcmp(paths[i], ".", strlen(paths[i])) == 0)
//         {
//             if (i == path_num - 1)
//             {
//                 int8_t retval = get_file_size(&cwd_table, paths[i], &filesize);

//                 if (retval == -1)
//                 {
//                     shell_put("File not found!\n", BIOS_RED);
//                     return false;
//                 }

//                 uint32_t parent_cluster_number = get_cluster_number(&cwd_table);
//                 char buffer[filesize];
//                 make_request(&req, buffer, filesize, parent_cluster_number, src_name, src_ext);
//                 int8_t retcode = sys_read(&req);
//                 if (retcode != 0)
//                 {
//                     return false;
//                 }
//                 return true;
//             }
//         }
//         else if (memcmp(src_ext, "\0\0\0", 3) != 0)
//         {
//             // Kalo dia file, tapi bukan end of dir
//             if (i != path_num - 1)
//             {
//                 shell_put("Invalid Path!\n", BIOS_RED);
//                 return false;
//             }
//             else
//             {
//                 // Read from bin then write to the desired path
//                 peek(&dts_copy, &cwd_table);
//                 uint32_t parent_cluster_number = get_cluster_number(&cwd_table);
//                 int8_t retval = get_file_size(&cwd_table, paths[i], &filesize);

//                 if (retval == -1)
//                 {
//                     shell_put("File not found!\n", BIOS_RED);
//                     return false;
//                 }

//                 char buffer[filesize];
//                 make_request(&req, buffer, filesize, parent_cluster_number, src_name, src_ext);
//                 retval = sys_read(&req);
//                 if (retval == 1)
//                 {
//                     shell_put("Input is not a file!\n", BIOS_RED);
//                     return false;
//                 }
//                 else if (retval == 3)
//                 {
//                     shell_put("File not found!\n", BIOS_RED);
//                     return false;
//                 }
//                 else if (retval == -1)
//                 {
//                     shell_put("Unexcpected error occurs\n", BIOS_RED);
//                     return false;
//                 }
//             }
//         }
//         // Kalo dia directory, maka push ke curr directory
//         else
//         {
//             make_request(&req, &cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, src_name, src_ext);
//             int8_t ret = sys_read_dir(&req);
//             if (ret != 0)
//             {
//                 shell_put("Invalid path!\n", BIOS_RED);
//                 return false;
//             }
//             push(&dts_copy, &cwd_table);
//             if (i == path_num - 1)
//             {
//                 int8_t retval = get_file_size(&cwd_table, paths[i], &filesize);

//                 if (retval == -1)
//                 {
//                     shell_put("File not found!\n", BIOS_RED);
//                     return false;
//                 }

//                 uint32_t parent_cluster_number = get_cluster_number(&cwd_table);
//                 char buffer[filesize];
//                 make_request(&req, buffer, filesize, parent_cluster_number, src_name, src_ext);
//                 int8_t retcode = sys_read(&req);
//                 if (retcode != 0)
//                 {
//                     return false;
//                 }
//                 return true;
//             }
//         }
//     }

//     // Parsing path to write the copy
//     deep_copy_dirtable_stack(&dts_copy, dts);
//     peek(&dts_copy, &cwd_table);
//     memset(paths, '\0', 12 * 128);
//     path_num = strparse(dest_path, paths, "/");

//     char dest_name[9];
//     char dest_ext[4];
//     for (uint8_t i = 0; i < path_num; i++)
//     {
//         uint32_t current_cluster_number = get_cluster_number(&cwd_table);
//         memset(dest_name, '\0', 9);
//         memset(dest_ext, '\0', 4);
//         parse_file_name(paths[i], dest_name, dest_ext);
//         // Kalo titik dua naik
//         if (strlen(paths[i]) == 2 && memcmp(paths[i], "..", strlen(paths[i])) == 0)
//         {
//             pop(&dts_copy);
//             if (i == path_num - 1)
//             {
//                 peek(&dts_copy, &cwd_table);
//                 uint32_t parent_cluster_number = get_cluster_number(&cwd_table);
//                 char buffer[filesize];
//                 make_request(&req, buffer, filesize, parent_cluster_number, src_name, src_ext);
//                 int8_t retcode = sys_write(&req);
//                 if (retcode != 0)
//                 {
//                     return false;
//                 }
//                 return true;
//             }
//         }
//         // Kalo titik satu skip
//         else if (strlen(paths[i]) == 1 && memcmp(paths[i], ".", strlen(paths[i])) == 0)
//         {
//             if (i == path_num - 1)
//             {
//                 uint32_t parent_cluster_number = get_cluster_number(&cwd_table);
//                 char buffer[filesize];
//                 make_request(&req, buffer, filesize, parent_cluster_number, src_name, src_ext);
//                 int8_t retcode = sys_write(&req);
//                 if (retcode != 0)
//                 {
//                     return false;
//                 }
//                 return true;
//             }
//         }
//         else if (memcmp(dest_ext, "\0\0\0", 3) != 0)
//         {
//             // Kalo dia file, tapi bukan end of dir
//             if (i != path_num - 1)
//             {
//                 shell_put("Invalid Path!\n", BIOS_RED);
//                 return false;
//             }
//             else
//             {
//                 peek(&dts_copy, &cwd_table);
//                 uint32_t parent_cluster_number = get_cluster_number(&cwd_table);
//                 char buffer[filesize];
//                 make_request(&req, buffer, filesize, parent_cluster_number, dest_name, dest_ext);
//                 int8_t retval = sys_write(&req);
//                 if (retval == 1)
//                 {
//                     shell_put("File with the same name already exist!\n", BIOS_RED);
//                     return false;
//                 }
//                 else if (retval == -1)
//                 {
//                     shell_put("Unexcpected error occurs\n", BIOS_RED);
//                     return false;
//                 }
//                 return true;
//             }
//         }
//         // Kalo dia directory, maka push ke curr directory
//         else
//         {
//             make_request(&req, &cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, dest_name, dest_ext);
//             int8_t ret = sys_read_dir(&req);
//             if (ret != 0)
//             {
//                 shell_put("Invalid path!\n", BIOS_RED);
//                 return false;
//             }
//             push(&dts_copy, &cwd_table);
//             if (i == path_num - 1)
//             {
//                 uint32_t parent_cluster_number = get_cluster_number(&cwd_table);
//                 char buffer[filesize];
//                 make_request(&req, buffer, filesize, parent_cluster_number, src_name, src_ext);
//                 int8_t retcode = sys_write(&req);
//                 if (retcode != 0)
//                 {
//                     return false;
//                 }
//                 return true;
//             }
//         }
//     }
//     return false;
// }

// void rm(char *path, struct DirTableStack *dts)
// {
//     // Initialize things
//     struct FAT32DirectoryTable cwd_table;
//     struct FAT32DriverRequest req;
//     struct DirTableStack dts_copy;

//     deep_copy_dirtable_stack(&dts_copy, dts);

//     char paths[12][128];

//     uint8_t path_num = strparse(path, paths, "/");
//     // Placeholder for FAT32DriverRequest

//     char name[9];
//     char ext[4];

//     int8_t retcode;

//     // Parse Path
//     for (uint8_t i = 0; i < path_num; i++)
//     {
//         peek(&dts_copy, &cwd_table);
//         uint32_t current_cluster_number = get_cluster_number(&cwd_table);
//         memset(name, '\0', 9);
//         memset(ext, '\0', 4);
//         parse_file_name(paths[i], name, ext);
//         // Kalo titik dua naik
//         if (strlen(paths[i]) == 2 && memcmp(paths[i], "..", strlen(paths[i])) == 0)
//         {
//             pop(&dts_copy);
//         }
//         // Kalo titik satu skip
//         else if (strlen(paths[i]) == 1 && memcmp(paths[i], ".", strlen(paths[i])) == 0)
//         {
//         }

//         else if (memcmp(ext, "\0\0\0", 3) != 0)
//         {
//             // Kalo dia file, tapi bukan end of dir
//             if (i != path_num - 1)
//             {
//                 shell_put_with_nextline("Invalid Path !", BIOS_RED);
//                 return;
//             }
//             else
//             {
//                 struct FAT32DriverRequest req;
//                 make_request(&req, NULL, sizeof(struct FAT32DirectoryTable), current_cluster_number, name, ext);
//                 retcode = sys_delete(&req);
//             }
//         }
//         // Kalo dia directory, maka push ke curr directory
//         else
//         {
//             // Kalau belum terakhir
//             if (i != path_num - 1)
//             {
//                 struct FAT32DriverRequest req;
//                 make_request(&req, &cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, name, "\0\0\0");
//                 retcode = sys_read_dir(&req);
//                 if (retcode != 0)
//                 {
//                     shell_put_with_nextline("Invalid Path !", BIOS_RED);
//                     return;
//                 }
//                 push(&dts_copy, &cwd_table);
//             }
//             // Kalau udah terakhir langsung delete
//             else
//             {
//                 make_request(&req, NULL, sizeof(struct FAT32DirectoryTable), current_cluster_number, name, ext);
//                 retcode = sys_delete(&req);
//             }
//         }
//     }

//     if (retcode == 0)
//     {
//         shell_put_with_nextline("File deleted !", BIOS_GREEN);
//     }
//     else if (retcode == 2)
//     {
//         shell_put_with_nextline("Folder is not empty!", BIOS_RED);
//     }
//     else
//     {
//         shell_put_with_nextline("Failed to delete file :(", BIOS_RED);
//     }
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
    uint32_t current_cluster_number;
    uint32_t parent_cluster_number;

    // Parse Path
    for (uint8_t i = 0; i < path_num; i++)
    {
        peek(&dts_copy, &cwd_table);
        current_cluster_number = get_cluster_number(&cwd_table);
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
        // Flash out all child
        parent_cluster_number = get_cluster_number(&cwd_table);

        make_request(&req, &cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, name, "\0\0\0");

        retcode = sys_read_dir(&req);

        if (retcode != 0)
        {
            shell_put_with_nextline("Failed to read directory !", BIOS_RED);
            return;
        }
        push(&dts_copy, &cwd_table);
        for (int i = 2; i < 64; i++)
        {
            if (cwd_table.table[i].user_attribute == UATTR_NOT_EMPTY)
            {
                if (cwd_table.table[i].attribute == ATTR_SUBDIRECTORY)
                {
                    rm(cwd_table.table[i].name, &dts_copy);
                }
                else
                {
                    make_request(&req, NULL, sizeof(struct FAT32DirectoryTable), current_cluster_number, cwd_table.table[i].name, cwd_table.table[i].ext);
                    retcode = sys_delete(&req);
                    if (retcode != 0)
                    {
                        shell_put_with_nextline("Failed to delete file :(", BIOS_RED);
                    }
                }
            }
        }
        make_request(&req, NULL, sizeof(struct FAT32DirectoryTable), parent_cluster_number, name, "\0\0\0");
        retcode = sys_delete(&req);
    }
    else
    {
        shell_put_with_nextline("Failed to delete file :(", BIOS_RED);
    }
}

void mv(char *src_path, char *dest_path, struct DirTableStack *dts)
{
    if (cp(src_path, dest_path, dts) == true)
    {
        rm(src_path, dts);
    }
}

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
        shell_put_with_nextline("root", BIOS_WHITE);
    }
}

void find_helper(char *name, char *ext, struct DirTableStack *dts)
{
    // Initialize things
    struct FAT32DirectoryTable cwd_table;

    struct DirTableStack dts_copy;
    deep_copy_dirtable_stack(&dts_copy, dts);
    peek(&dts_copy, &cwd_table);

    for (int i = 2; i < 64; i++)
    {

        // Kalau dia ada isinya, cleanup isi
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
                pop(&dts_copy);
                peek(&dts_copy, &cwd_table);
            }
        }
    }
}

void help()
{
    shell_put("List Commands: \n", BIOS_WHITE);
    shell_put("1. cd : Mengganti current working directory\n", BIOS_WHITE);
    shell_put("   penggunaan: cd [nama_directory]\n", BIOS_YELLOW);
    shell_put("2. ls : Menuliskan isi current working directory\n", BIOS_WHITE);
    shell_put("   penggunaan: ls\n", BIOS_YELLOW);
    shell_put("3. mkdir : Membuat sebuah folder kosong baru\n", BIOS_WHITE);
    shell_put("   penggunaan: mkdir [nama_directory]\n", BIOS_YELLOW);
    shell_put("4. cat : Menuliskan sebuah file sebagai text file ke layar \n", BIOS_WHITE);
    shell_put("   penggunaan: cat [nama_file]\n", BIOS_YELLOW);
    shell_put("5. cp : Mengcopy suatu file\n", BIOS_WHITE);
    shell_put("   penggunaan: cp [source_dir] [dest_dir]\n", BIOS_YELLOW);
    shell_put("6. rm : Menghapus suatu file\n", BIOS_WHITE);
    shell_put("   penggunaan: rm [nama_file]\n", BIOS_YELLOW);
    shell_put("7. mv : Memindah dan merename lokasi file/folder\n", BIOS_WHITE);
    shell_put("   penggunaan: mv [source_dir] [dest_dir]\n", BIOS_YELLOW);
    shell_put("8. find : Mencari file/folder dengan nama yang sama diseluruh file system\n", BIOS_WHITE);
    shell_put("   penggunaan: find [nama_directory]\n", BIOS_YELLOW);
    shell_put("9. ps : Menampilkan semua process yang sedang berjalan", BIOS_WHITE);
    shell_put("   penggunaan: ps\n", BIOS_YELLOW);
    shell_put("10. exec : Menjalankan program", BIOS_WHITE);
    shell_put("   penggunaan: exec [nama_program]\n", BIOS_YELLOW);
    shell_put("11. kill : Menghentikan program", BIOS_WHITE);
    shell_put("   penggunaan: kill [pid]\n", BIOS_YELLOW);
    shell_put("12. clear: Menghapus screen saat ini", BIOS_WHITE);
    shell_put("   penggunaan: clear\n", BIOS_YELLOW);
}

void clear()
{
    sys_clear();
}

void ps()
{
    syscall(9, 0, 0, 0);
}
void exec(char *filename, struct DirTableStack *dts)
{
    // Initialize things
    struct FAT32DirectoryTable cwd_table;
    struct FAT32DriverRequest req;
    uint32_t filesize;
    peek(dts, &cwd_table);

    // Find the filesize
    int8_t retval = get_file_size(&cwd_table, filename, &filesize);

    if (retval != 0)
    {
        shell_put("Unexcpected error occurs\n", BIOS_RED);
        return;
    }
    char buffer[filesize];
    uint32_t parent_cluster_number = get_cluster_number(&cwd_table);
    char name[9];
    char ext[4];
    memset(name, '\0', 9);
    memset(ext, '\0', 4);
    parse_file_name(filename, name, ext);
    make_request(&req, buffer, filesize, parent_cluster_number, name, ext);

    syscall(11, (uint32_t)&req, (uint32_t)&retval, 0);

    if (retval != 0)
    {
        shell_put("Fails to execute process!\n", BIOS_RED);
        return;
    }
}

void clock()
{
    // Find the root directory
    struct FAT32DirectoryTable root_table;
    struct FAT32DriverRequest req;
    int8_t retcode;
    make_request(&req, &root_table, sizeof(struct FAT32DirectoryTable), ROOT_CLUSTER_NUMBER, "root\0\0\0\0", "\0\0\0");

    retcode = sys_read_dir(&req);

    if (retcode != 0)
    {
        shell_put_with_nextline("Failed to read root directory !", BIOS_RED);
        return;
    }

    uint32_t filesize;

    retcode = get_file_size(&root_table, "clock\0\0\0", &filesize);

    if (retcode != 0)
    {
        shell_put("Unexcpected error occurs\n", BIOS_RED);
        return;
    }

    char buffer[filesize];
    uint32_t parent_cluster_number = get_cluster_number(&root_table);
    char name[9];
    char ext[4];
    memset(name, '\0', 9);
    memset(ext, '\0', 4);
    make_request(&req, buffer, filesize, parent_cluster_number, "clock\0\0\0", "\0\0\0");

    syscall(11, (uint32_t)&req, (uint32_t)&retcode, 0);

    if (retcode != 0)
    {
        shell_put("Fails to execute process!\n", BIOS_RED);
        return;
    }
}
void kill(char *pid)
{
    // Convert PID string to integer
    int32_t int_pid = 0;
    bool conversion_success = true;
    char *str = pid;
    int sign = 1;

    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    else if (*str == '+')
    {
        str++;
    }

    if (*str == '\0')
    {
        conversion_success = false; // Empty string after sign
    }

    while (*str)
    {
        if (*str < '0' || *str > '9')
        {
            conversion_success = false; // Invalid character
            break;
        }
        if (int_pid > (INT32_MAX - (*str - '0')) / 10)
        {
            conversion_success = false; // Overflow
            break;
        }
        int_pid = int_pid * 10 + (*str - '0');
        str++;
    }

    if (conversion_success)
    {
        int_pid *= sign;
    }
    else
    {
        shell_put("Invalid PID!\n", BIOS_RED);
        return;
    }

    int8_t retcode;

    syscall(12, (uint32_t)int_pid, (uint32_t)&retcode, 0);

    if (retcode == 1)
    {
        shell_put("Process Not Found!\n", BIOS_RED);
    }
}