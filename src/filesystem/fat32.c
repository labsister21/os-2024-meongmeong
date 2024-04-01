#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../header/stdlib/string.h"
#include "../header/filesystem/fat32.h"

extern struct FAT32DriverState fat32_driver_state;
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
    struct FAT32DirectoryEntry val = {0};
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
    for (int i = 0; i < 3; i++)
    {
        dir_table->table[i].ext[i] = '\0';
    }
    uint32_t res = get_empty_cluster();
    if (res)
    {
        dir_table->table[0].cluster_high = (uint16_t)res;
        dir_table->table[0].cluster_low = (uint16_t)res >> 16;
    }
    // Search entry for parent
    struct FAT32DirectoryEntry *parent;
    read_clusters(parent, parent_dir_cluster, 1);
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


bool is_available_slot(uint32_t parent_dir_cluster)
{
    struct FAT32DirectoryTable *info;
    bool found;
    int i = 0;
    int idx;
    read_clusters(info, parent_dir_cluster, 1);
    while (!found && i < 64)
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
    uint16_t i = 3;
    uint16_t idx;
    while (!found && i < CLUSTER_MAP_SIZE)
    {
        if (fat32_driver_state.fat_table.cluster_map[i] == 0)
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
