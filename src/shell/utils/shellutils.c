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

void parse_file_name(char *filename, char *name, char *ext)
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

void parse_user_input(char *buffer, char *command_name, char *args)
{
    int i = 0;
    while (buffer[i] != ' ' && buffer[i] != '\0')
    {
        command_name[i] = buffer[i];
        i++;
    }
    command_name[i] = '\0';
    if (buffer[i] == ' ')
    {
        i++;
        int j = 0;
        while (buffer[i] != '\0')
        {
            args[j] = buffer[i];
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

void shell_put_with_nextline(char *str, uint32_t color)
{
    syscall(6, (uint32_t)str, strlen(str), color);
    shell_put("\n", color);
}

size_t parse_num_args(char *args)
{
    size_t num_args = 0;
    size_t i = 0;
    while (args[i] != '\0')
    {
        if (args[i] == ' ')
        {
            num_args++;
        }
        i++;
    }
    return num_args + 1;
}

void make_request(struct FAT32DriverRequest *request, void *buf, uint32_t buffer_size, uint32_t parent_cluster_number, char *name, char *ext)
{
    request->buf = buf;
    request->buffer_size = buffer_size;
    request->parent_cluster_number = parent_cluster_number;
    memcpy(request->name, name, 8);
    memcpy(request->ext, ext, 3);
}

void update_cwd_table(struct FAT32DirectoryTable *cwd_table)
{
    int8_t retcode;
    struct FAT32DriverRequest cwd_request;
    uint32_t current_cluster_number = cwd_table->table[0].cluster_low | ((uint32_t)cwd_table->table[0].cluster_high) << 16;
    make_request(&cwd_request, cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, cwd_table->table[0].name, "\0\0\0");
    syscall(1, (uint32_t)&cwd_request, (uint32_t)&retcode, 0);
    if (retcode != 0)
    {
        shell_put("Unexpected Error Occured, please quit this program! ", BIOS_RED);
    }
}

uint32_t get_cluster_number(struct FAT32DirectoryTable *cwd) { return cwd->table[0].cluster_low | ((uint32_t)cwd->table[0].cluster_high) << 16; }

int8_t sys_read(struct FAT32DriverRequest *request)
{
    int8_t retcode;
    syscall(0, (uint32_t)request, (uint32_t)&retcode, 0);
    return retcode;
}

int8_t sys_read_dir(struct FAT32DriverRequest *request)
{
    int8_t retcode;
    syscall(1, (uint32_t)request, (uint32_t)&retcode, 0);
    return retcode;
}

int8_t sys_write(struct FAT32DriverRequest *request)
{
    int8_t retcode;
    syscall(2, (uint32_t)request, (uint32_t)&retcode, 0);
    return retcode;
}

int8_t sys_delete(struct FAT32DriverRequest *request)
{
    int8_t retcode;
    syscall(3, (uint32_t)request, (uint32_t)&retcode, 0);
    return retcode;
}
