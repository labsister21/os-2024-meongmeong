#ifndef _CAT_H
#define _CAT_H

#include "../stdlib/string.h"     // memory handling
#include "../stdlib/string-lib.h" // string handling
#include "../driver/framebuffer.h"
#include "../filesystem/fat32.h"

#define COLOR_BLUE 0x09
#define COLOR_GREEN 0x02
#define COLOR_CYAN 0x0B
#define COLOR_GRAY 0x07
#define COLOR_RED 0x0C
#define COLOR_MAGENTA 0x0D
#define COLOR_YELLOW 0x0E

extern char current_directory[];
extern uint32_t cwd_cluster_number;
extern struct FAT32DirectoryTable cwd_table;

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

/**
 * @brief Display buf in framebuffer
 *
 * @param buf
 * @param color bg and fg color
 */
void puts(char *buf, uint8_t color);

/**
 * @brief Sleep system implementation by for loop doing nothing
 *
 * @param microseconds time of sleep
 */
void sleep(int microseconds);

/**
 * @brief Display splashscreen in framebuffer
 *
 */
void splashScreen();

/**
 * @brief Execute matching command with user input
 *
 * @param buf user input
 */
void executeCommand(char *buf);

void copy(char *src_name, char *src_ext, uint32_t src_parent_number, char *target_name, char *target_ext, uint32_t target_parent_number);

/**
 * Parses a path into individual parts and stores them
 * in an array of strings. The parts are separated by forward slashes ('/').
 * Any consecutive forward slashes in the path are treated as a single delimiter.
 * The number of parts found is stored in the provided integer pointer.
 *
 * @param path The path to parse.
 * @param parts An array of strings to store the parts in.
 * @param numParts A pointer to an integer to store the number of parts found.
 */
void parsePath(const char *path, char **parts, int *numParts);

/**
 * Implementation of the `mkdir` command
 */
void mkdir(char *dirname);

/**
 * Implementation of the `cat` command
 */
void extractFilenameExtension(char *filename, char *name, char *extension); // HALO
void cat(char *filePath);                                                   // HALO

/**
 * Implementation of the `cd` command
 */
void cd(char *filePath); // HALO

/**
 * Implementation of the `ls` command
 */
void ls();                                                                                                                                // HALO
void copy(char *src_name, char *src_ext, uint32_t src_parent_number, char *target_name, char *target_ext, uint32_t target_parent_number); // HALO

/**
 * If source has folder but no -r flag: ERROR
 * If target exists:
 *  - target_directory: copy all source into target_directory
 *  - target_file:
 *      - single source: overwrite
 *      - multiple source: ERROR
 * If target does not exist:
 *  - If multiple source: ERROR
 *  - If single source:
 *      - If source_directory: create new directory
 *      - If source_file: create new file
 */
void cp(char *command);

void remove(char *name, char *ext, uint32_t parent_number);

void rm(char *command);

void executeCommand(char *buf);

/**
 * @brief Search path of file/folder with the same name. Only accept 1 argument of file/folder name.
 *
 * @param buf user input
 */
void whereisCommand(char *buf);

/**
 * @brief DFS search implementation for file structure. Display path in framebuffer if found.
 *
 * @param folderAddress folder address cluster to be searched
 * @param searchName name of file/folder
 * @param isFound 0 if file/folder isn't found, 1 if found
 */
void DFSsearch(uint32_t folderAddress, char *searchName, int8_t *isFound);

/**
 * @brief construct path from root to folder
 *
 * @param clusterAddress cluster address of folder
 */
void constructPath(uint32_t clusterAddress);

/**
 * @brief Move and rename file/folder
 *
 * @param buf user input
 */
void moveCommand(char *buf);

#endif