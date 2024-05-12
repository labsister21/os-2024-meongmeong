#include "../../header/shell/commands/commands.h"
#include "../../header/shell/utils/shellutils.h"

void cd(char *dir)
{
}

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

void mkdir(char *dir)
{
}

void cat(char *filename, char *ext)
{
    // Validate filename
}

void cp(char *filename)
{
}

void rm(char *filename, char *ext)
{
}

void mv(char *filename, char *dirname)
{
}

void find(char *filename)
{
}