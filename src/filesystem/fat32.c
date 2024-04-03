#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../header/stdlib/string.h"
#include "../header/filesystem/fat32.h"

extern struct FAT32DriverState fat32_driver_state;
typedef enum
{
    DIRECTORY,
    FILE
} FileType;
const uint8_t fs_signature[BLOCK_SIZE] = {
    'C',
    'o',
    'u',
    'r',
    's',
    'e',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'D',
    'e',
    's',
    'i',
    'g',
    'n',
    'e',
    'd',
    ' ',
    'b',
    'y',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    'L',
    'a',
    'b',
    ' ',
    'S',
    'i',
    's',
    't',
    'e',
    'r',
    ' ',
    'I',
    'T',
    'B',
    ' ',
    ' ',
    'M',
    'a',
    'd',
    'e',
    ' ',
    'w',
    'i',
    't',
    'h',
    ' ',
    '<',
    '3',
    ' ',
    ' ',
    ' ',
    ' ',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '-',
    '2',
    '0',
    '2',
    '4',
    '\n',
    [BLOCK_SIZE - 2] = 'O',
    [BLOCK_SIZE - 1] = 'k',
};

uint32_t cluster_to_lba(uint32_t cluster)
{
    return BOOT_SECTOR + cluster * CLUSTER_BLOCK_COUNT;
}

//
void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster)
{
    // Initialize all entries to 0
    struct FAT32DirectoryEntry val = {0};

    // Set all entries to empty value
    for (int i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        dir_table->table[i] = val;
    }

    // Create entry 0 (for himself)
    // Important data = name, cluster high, clusterlow, attribute, user attribute, extension
    for (int i = 0; i < 8; i++)
    {
        dir_table->table[0].name[i] = name[i];
    }
    dir_table->table[0].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[0].user_attribute = UATTR_NOT_EMPTY;
    // Get the first empty cluster
    uint32_t res = get_empty_cluster();
    if (res != -1)
    {
        dir_table->table[0].cluster_low = (uint16_t)res;
        dir_table->table[0].cluster_high = (uint16_t)(res >> 16);
    }

    // Case for root folder , init the root folder
    if (parent_dir_cluster == ROOT_CLUSTER_NUMBER && strncmp(name, "root\0\0\0\0", 8) == 0)
    {
        // Create entry
        dir_table->table[1].cluster_high = (uint16_t)0;
        dir_table->table[1].cluster_low = (uint16_t)ROOT_CLUSTER_NUMBER;
        dir_table->table[1].attribute = ATTR_SUBDIRECTORY;
        dir_table->table[1].user_attribute = UATTR_NOT_EMPTY;
        memcpy(dir_table->table[1].name, name, 8);
    }
    else
    {
        // Get the directory table for parent
        struct FAT32DirectoryTable parent;
        read_clusters(&parent, parent_dir_cluster, 1);
        // Copy parent entry to the new entry
        struct FAT32DirectoryEntry *parentEntry = &parent.table[0];
        dir_table->table[1] = *parentEntry;
    }
}

void create_fat32(void)
{
    write_blocks(fs_signature, BOOT_SECTOR, 1);
    fat32_driver_state.fat_table.cluster_map[0] = CLUSTER_0_VALUE;
    fat32_driver_state.fat_table.cluster_map[1] = CLUSTER_1_VALUE;
    fat32_driver_state.fat_table.cluster_map[2] = FAT32_FAT_END_OF_FILE;

    for (int i = 3; i < CLUSTER_MAP_SIZE; i++)
    {
        fat32_driver_state.fat_table.cluster_map[i] = FAT32_FAT_EMPTY_ENTRY;
    }
    write_clusters(fat32_driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
    struct FAT32DirectoryTable root_folder;
    init_directory_table(&root_folder, "root\0\0\0\0", ROOT_CLUSTER_NUMBER);
    write_clusters(&root_folder, ROOT_CLUSTER_NUMBER, 1);
}

void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    write_blocks(ptr, cluster_to_lba(cluster_number), cluster_count * CLUSTER_BLOCK_COUNT);
}

void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    read_blocks(ptr, cluster_to_lba(cluster_number), cluster_count * CLUSTER_BLOCK_COUNT);
}

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
            // read_clusters(request.buf, dir_cluster, 1);
            read_clusters(&fat32_driver_state.cluster_buf, dir_cluster, 1);

            return 0; // Success
        }
    }

    // tidak ditemukan
    return 2;
}

int8_t read(struct FAT32DriverRequest request)
{
    struct FAT32DirectoryEntry *entry;
    dirtable_search(entry, request.name, request.ext, request.parent_cluster_number);

    // Not a file
    if (entry->attribute == ATTR_SUBDIRECTORY)
    {
        return 1;
    }
    // Not enough bffer
    if (request.buffer_size < entry->filesize)
    {
        return 2;
    }
    // Not found
    if (entry->user_attribute != UATTR_NOT_EMPTY)
    {
        return 3;
    }

    // Found
    uint32_t cluster_number = ((uint32_t)entry->cluster_high) << 16 | (entry->cluster_low);
    if (cluster_number)
    {
        int itr = 0;
        // partition
        while (cluster_number != FAT32_FAT_END_OF_FILE)
        {
            read_clusters(request.buf + itr * CLUSTER_SIZE, cluster_number, 1);
            itr++;
            cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
        }
        return 0;
    }

    // Unknown Errors
    return -1;
}

int8_t delete(struct FAT32DriverRequest request)
{
    //Search for the entry in the desired parent directory
    struct FAT32DirectoryEntry *entry;
    dirtable_search(entry, request.name, request.ext, request.parent_cluster_number);

    uint32_t actual_cluster_number = ((uint32_t)entry->cluster_high) << 16 | (entry->cluster_low);

    // Case Not Found
    if (entry->user_attribute != UATTR_NOT_EMPTY)
    {
        return 1;
    }
    // Case the request is a folder and the folder is not empty
    if (is_directory(ROOT_CLUSTER_NUMBER, actual_cluster_number) && !is_directory_empty(actual_cluster_number))
    {
        return 2;
    }
    // Case for found and parent is a valid directory
    if (is_directory(ROOT_CLUSTER_NUMBER, request.parent_cluster_number))
    {
        // Delete from Parent Table Entry
        // Search for the Parent Table
        read_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);
        struct FAT32DirectoryEntry empty = {0};

        // search for the corresponding entry
        for (int i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
        {
            if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, request.name, 8) == 0 && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, request.ext, 3) == 0)
            {
                fat32_driver_state.dir_table_buf.table[i] = empty;
                break;
            }
        }
        // write the parent table back to the disk
        write_clusters(&fat32_driver_state.dir_table_buf, request.parent_cluster_number, 1);

        // Delete from FAT Table
        clear_cluster(actual_cluster_number);
        write_clusters(fat32_driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
        return 0;
    }

    // Case Unknown
    return -1;
}

/* Helpers Function Section */
bool is_available_entry(uint32_t parent_dir_cluster)
{
    struct FAT32DirectoryTable *info;
    bool found;
    int i = 2;
    int idx;
    read_clusters(info, parent_dir_cluster, 1);
    while (!found && i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry))
    {
        if (info->table->user_attribute == 0)
        {
            found = true;
            idx = i;
        }
        i++;
    }
    if (!found)
    {
        return false;
    }
    else
    {
        return true;
    }
}

uint32_t get_empty_cluster()
{
    bool found;
    uint16_t i = 2;
    uint16_t idx;
    while (!found && i < CLUSTER_MAP_SIZE)
    {
        if (fat32_driver_state.fat_table.cluster_map[i] == FAT32_FAT_EMPTY_ENTRY)
        {
            found = true;
            idx = i;
        }
        i++;
    }
    if (!found)
    {
        return -1;
    }
    else
    {
        return idx;
    }
}

void createEntry(struct FAT32DirectoryEntry *entry, char *name, char *ext, FileType type, uint32_t parent_dir_cluster, uint32_t filesize)
{
    // Common code for both directory and file entries
    uint32_t res = get_empty_cluster();
    if (res == -1)
    {
        return;
    }
    entry->cluster_high = (uint16_t)res >> 16;
    entry->cluster_low = (uint16_t)res;
    memcpy(entry->name, name, 8);
    memcpy(entry->ext, ext, 3);
    entry->user_attribute = UATTR_NOT_EMPTY;
    // Set attributes based on file type
    if (type == DIRECTORY)
    {
        entry->attribute = ATTR_SUBDIRECTORY;
        entry->filesize = 0; // Directories have zero file size
    }
    else if (type == FILE)
    {
        entry->attribute = 0; // Default attribute for files
        entry->filesize = filesize;
    }
}

bool is_directory_empty(uint32_t dir_cluster)
{
    // Get the directory table
    struct FAT32DirectoryTable *info;

    // Start the index at 2 because the first two entries are reserved
    int i = 2;
    read_clusters(info, dir_cluster, 1);

    // iterate through all entris from 2 - 63, check if there is any entry that is not empty
    while (i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry))
    {
        if (info->table[i].user_attribute == UATTR_NOT_EMPTY)
        {
            return false;
        }
        i++;
    }
    return true;
}

// Check whether or not a file or folder with the same name and extention exist in the parent directory
void dirtable_search(struct FAT32DirectoryEntry *entry, char *name, char *ext, uint32_t parent_cluster_number)
{
    // This assumes that no file have an extention of \0\0\0 so folder and file will definitely be diffrentiated by its extention
    read_clusters(fat32_driver_state.dir_table_buf.table, parent_cluster_number, 1);
    for (int i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, name, 8) == 0 && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, ext, 3) == 0)
        {
            *entry = fat32_driver_state.dir_table_buf.table[i];
        }
    }
    // assign entry with zero valued struct if not found
    *entry = (struct FAT32DirectoryEntry){0};
}

// when first called, we will call root as it parent_cluster
bool is_directory(uint32_t parent_cluster, uint32_t dir_cluster)
{
    struct FAT32DirectoryTable parent_directory_table;
    read_clusters(&parent_directory_table, parent_cluster, 1);
    bool flag = false;
    for (int i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (parent_directory_table.table[i].attribute == ATTR_SUBDIRECTORY)
        {
            uint32_t current_cluster_checked = parent_directory_table.table[i].cluster_low | ((uint32_t)parent_directory_table.table[i].cluster_high) << 16;
            if ((current_cluster_checked) == dir_cluster)
            {
                return true;
            }
            else
            {
                flag = is_directory(current_cluster_checked, dir_cluster);
                if (flag)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void clear_cluster(uint32_t cluster_number)
{
    uint32_t cluster = cluster_number;

    while (cluster != FAT32_FAT_END_OF_FILE)
    {
        // Retrieve the next cluster value
        uint32_t next = fat32_driver_state.fat_table.cluster_map[cluster];

        // Clear the current cluster entry
        fat32_driver_state.fat_table.cluster_map[cluster] = FAT32_FAT_EMPTY_ENTRY;

        // Move to the next cluster
        cluster = next;
    }
}