#include "../../header/shell/commands/commands.h"

void execute_commands(char* buffer)
{
    char command_name[256];
    char args[256];
    memset(command_name, 0, 256);
    memset(args, 0, 256);

    parse_user_input(buffer, command_name, args);
    if (memcmp(command_name, "ls", 2))
    {
        ls();
        // return 0;
    }
    // return 1;
    // else
    // {
        
    //     shell_put()
    // }
}

// void cd(char *dir)
// {

// }


void ls()
    {
    struct FAT32DirectoryTable cwd;
    syscall(9, (uint32_t)&cwd, 0, 0);

    for (int i = 2; i < 64; i++)
    {
        if (cwd.table[i].user_attribute == UATTR_NOT_EMPTY)
        {

            shell_put(cwd.table[i].name, 0xF);
            shell_put(".", 0xF);
            shell_put(cwd.table[i].ext, 0xF);
            shell_put("\n", 0xF);
        }
    }
}

// void mkdir(char *dir)
// {
// }

// void cat(char *filename, char *ext)
// {
//     // Validate filename
// }

// void cp(char *filename)
// {
// }

// void rm(char *filename, char *ext)
// {
// }

// void mv(char *filename, char *dirname)
// {
// }

// void find(char *filename)
// {
// }