#ifndef SHELLUTILS_H
#define SHELLUTILS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../stdlib/string-lib.h"
#include "../../filesystem/fat32.h"

#define BIOS_BLACK 0x0
#define BIOS_BLUE 0x1
#define BIOS_GREEN 0x2
#define BIOS_CYAN 0x3
#define BIOS_RED 0x4
#define BIOS_MAGENTA 0x5
#define BIOS_BROWN 0x6
#define BIOS_LIGHT_GRAY 0x7
#define BIOS_DARK_GRAY 0x8
#define BIOS_LIGHT_BLUE 0x9
#define BIOS_LIGHT_GREEN 0xA
#define BIOS_LIGHT_CYAN 0xB
#define BIOS_LIGHT_RED 0xC
#define BIOS_LIGHT_MAGENTA 0xD
#define BIOS_YELLOW 0xE
#define BIOS_WHITE 0xF

/**
 * @brief untuk syscal dari usershell
 *
 * @param eax
 * @param ebx
 * @param ecx
 * @param edx
 */
void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);
/**
 * @brief menambah null terminator pada string yang belum memiliki null terminator
 *
 */

void add_null_terminator(char *str);

/**
 * @brief memecah filename menjadi name dan ext
 *
 * @param filename
 * @param name
 * @param ext
 */

void parse_file_name(char *filename, char *name, char *ext);
/**
 * @brief Memecah buffer menjadi command_name dan args
 *
 * @param buffer
 * @param command_name
 * @param args
 */
void parse_user_input(char *buffer, char *command_name, char *args);
/**
 * @brief Helper untuk shell agar mudah put
 *
 * @param str
 * @param color
 */
void shell_put(char *str, uint32_t color);
/**
 * @brief get the number of arguments
 *
 */
size_t parse_num_args(char *args);
/**
 * @brief Helper to make request
 *
 * @param request
 * @param buf
 * @param buffer_size
 * @param parent_cluster_number
 * @param name
 * @param ext
 */
void make_request(struct FAT32DriverRequest *request, void *buf, uint32_t buffer_size, uint32_t parent_cluster_number, char *name, char *ext);
/**
 * @brief Update cwd table
 *
 * @param cwd
 */
void update_cwd_table(struct FAT32DirectoryTable *cwd);
/**
 * @brief helper of systemcall 0
 *
 * @param request
 * @return int8_t
 */
int8_t sys_read(struct FAT32DriverRequest *request);
/**
 * @brief helper of systemcall 1
 *
 * @param request
 * @return int8_t
 */
int8_t sys_read_dir(struct FAT32DriverRequest *request);
/**
 * @brief helper of systemcall 2
 *
 * @param request
 * @return int8_t
 */
int8_t sys_write(struct FAT32DriverRequest *request);
/**
 * @brief helper of systemcall 3
 *
 * @param request
 * @return int8_t
 */
int8_t sys_delete(struct FAT32DriverRequest *request);


#endif