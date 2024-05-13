#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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
 * @param ext
 * @param name
 */

void parse_file_name(char *filename, char *ext, char *name);
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
 * @brief
 *
 * @param str
 */
size_t strlen(char *str);