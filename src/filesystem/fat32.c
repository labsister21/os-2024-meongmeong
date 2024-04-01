#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../header/stdlib/string.h"
#include "../header/filesystem/fat32.h"

const uint8_t fs_signature[BLOCK_SIZE] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '4', '\n',
    [BLOCK_SIZE-2] = 'O',
    [BLOCK_SIZE-1] = 'k',
};

/*
FAT32DriverState
Buatlah satu definisi static FAT32DriverState untuk pada fat32.c. Variabel ini akan digunakan driver FAT32 untuk menyimpan state file system pada memory (RAM). Nantinya atribut dari struktur data ini digunakan untuk semua operasi CRUD.
*/
struct FAT32DriverState fat32_driver_state;

/**
 * Initialize file system driver state, if is_empty_storage() then create_fat32()
 * Else, read and cache entire FileAllocationTable (located at cluster number 1) into driver state
 */
void initialize_filesystem_fat32()
{
    // Jika empty -> membuat fat32
    if (is_empty_storage())
    {
        create_fat32();
    }
    // Jika tidak empty -> membaca driver state fat32
    else
    {
        read_clusters(&fat32_driver_state.fat_table, FAT_CLUSTER_NUMBER, 1);
    }
}

/* -- CRUD Operation -- */

/**
 *  FAT32 Folder / Directory read
 *
 * @param request buf point to struct FAT32DirectoryTable,
 *                name is directory name,
 *                ext is unused,
 *                parent_cluster_number is target directory table to read,
 *                buffer_size must be exactly sizeof(struct FAT32DirectoryTable)
 * @return Error code: 0 success - 1 not a folder - 2 not found - -1 unknown
 */
int8_t read_directory(struct FAT32DriverRequest request)
{
    struct FAT32DirectoryTable parent_dir_table;

    // tidak diketahui
    if (request.buffer_size != sizeof(parent_dir_table))
    {
        return -1;
    }

    read_clusters(&parent_dir_table, request.parent_cluster_number, 1);

    struct FAT32DirectoryEntry dir_entry;
    format_directory_name(request.name, dir_entry.name);

    // Loop through the entries to find the directory
    for (int i = 0; i < (CLUSTER_SIZE / sizeof(dir_entry)); i++)
    {
        struct FAT32DirectoryEntry *entry = &parent_dir_table.table[i];

        // Check if entry is the directory we are looking for
        if (strncmp(entry->name, dir_entry.name, 8) == 0)
        {
            // Found the directory

            // Check if it is actually a directory and not a file
            if ((entry->attribute & ATTR_SUBDIRECTORY) == 0)
            {
                return 1; // Not a directory
            }

            // Read the directory's contents into the buffer
            uint32_t dir_cluster = ((uint32_t)entry->cluster_high << 16) | entry->cluster_low;
            read_clusters(request.buf, dir_cluster, 1);

            return 0; // Success
        }
    }

    // tidak ditemukan
    return 2;
}

/**
 * FAT32 read, read a file from file system.
 *
 * @param request All attribute will be used for read, buffer_size will limit reading count
 * @return Error code: 0 success - 1 not a file - 2 not enough buffer - 3 not found - -1 unknown
 */
int8_t read(struct FAT32DriverRequest request);

/**
 * FAT32 write, write a file or folder to file system.
 *
 * @param request All attribute will be used for write, buffer_size == 0 then create a folder / directory
 * @return Error code: 0 success - 1 file/folder already exist - 2 invalid parent cluster - -1 unknown
 */
int8_t write(struct FAT32DriverRequest request);

/**
 * FAT32 delete, delete a file or empty directory (only 1 DirectoryEntry) in file system.
 *
 * @param request buf and buffer_size is unused
 * @return Error code: 0 success - 1 not found - 2 folder is not empty - -1 unknown
 */
int8_t delete(struct FAT32DriverRequest request);