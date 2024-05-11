#include <stdint.h>
#include "header/filesystem/fat32.h"
#include "header/shell/commands/commands.h"
#include "header/shell/utils/shellutils.h"

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
    //     syscall(6, (uint32_t) "owo\n", 4, 0xF);

    char cwd;
    // Syscall 0 = read
    // Syscall 1 = read_directory
    // Syscall 2 = write
    // Syscall 3 = delete
    // Syscall 4 = get_keyboard_buffer
    // Syscall 5 = putchar
    // Syscall 6 = puts
    // Syscall 7 = activate keyword
    // Syscall 8 = getcwd
    // syscall(8, (uint32_t)&cwd, 0, 0);
    // syscall(6, (uint32_t)&cwd, 4, 0xF);
    // syscall(7, 0, 0, 0);
    struct FAT32DirectoryTable cwdb;
    syscall(9, (uint32_t)&cwdb, 0, 0);

    for (int i = 0; i < 64; i++)
    {
        if (cwdb.table[i].user_attribute == UATTR_NOT_EMPTY)
        {

            shellPut(cwdb.table[i].name, 0xF);
            shellPut(".", 0xF);
            shellPut(cwdb.table[i].ext, 0xF);
            shellPut("\n", 0xF);
        }
    }
    while (true)
    {
        char buf;
        syscall(4, (uint32_t)&buf, 0, 0);
        if (buf)
        {
            syscall(5, buf, 0xF, 0);
        }
    }

    return 0;
}
