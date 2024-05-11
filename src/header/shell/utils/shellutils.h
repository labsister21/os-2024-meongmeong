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

void addNullTerminator(char *str);

/**
 * @brief memecah filename menjadi name dan ext
 *
 * @param filename
 * @param ext
 * @param name
 */

void parseFileName(char *filename, char *ext, char *name);
/**
 * @brief Memecah command menjadi commandName dan args
 *
 * @param command
 * @param commandName
 * @param args
 */
void parseCommand(char *command, char *commandName, char *args);
/**
 * @brief Helper untuk shell agar mudah put
 *
 * @param str
 * @param color
 */
void shellPut(char *str, uint32_t color);
/**
 * @brief
 *
 * @param str
 */
size_t strlen(char *str);
