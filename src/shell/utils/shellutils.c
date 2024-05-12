#include "../../header/shell/utils/shellutils.h"

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx)
{
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}

void add_null_terminator(char *str)
{
    // Make sure fill all until 8
    int counter = 8;
    while (*str != '\0')
    {
        str++;
        counter--;
    }
    while (counter > 0)
    {
        *str = '\0';
        str++;
        counter--;
    }
}

void parse_file_name(char *filename, char *ext, char *name)
{
    int i = 0;
    while (filename[i] != '.' && filename[i] != '\0')
    {
        name[i] = filename[i];
        i++;
    }
    name[i] = '\0';
    if (filename[i] == '.')
    {
        i++;
        int j = 0;
        while (filename[i] != '\0')
        {
            ext[j] = filename[i];
            i++;
            j++;
        }
        ext[j] = '\0';
    }
}

void parse_command(char *command, char *commandName, char *args)
{
    int i = 0;
    while (command[i] != ' ' && command[i] != '\0')
    {
        commandName[i] = command[i];
        i++;
    }
    commandName[i] = '\0';
    if (command[i] == ' ')
    {
        i++;
        int j = 0;
        while (command[i] != '\0')
        {
            args[j] = command[i];
            i++;
            j++;
        }
        args[j] = '\0';
    }
}

void shell_put(char *str, uint32_t color)
{
    syscall(6, (uint32_t)str, strlen(str), color);
}

size_t strlen(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        i++;
    }
    return i;
}