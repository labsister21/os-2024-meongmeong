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
        dir_table->table[0].cluster_high = (uint16_t) (res >> 16);
        dir_table->table[0].cluster_low = (uint16_t) res;
    }
    // Search entry for parent
    // Special case for root directory
    char root_name[] = {'r', 'o', 'o', 't'};
    if (memcmp(name, root_name, 4) != 0)
    {
        struct FAT32DirectoryEntry *parent;
        read_clusters(parent, parent_dir_cluster, 1);
        dir_table->table[1] = *parent;
    }
    else
    {
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
            dir_table->table[0].cluster_high = (uint16_t) (res >> 16);
            dir_table->table[0].cluster_low = (uint16_t) res;
        }
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

bool is_empty_storage(void)
{
    uint8_t compared_buffer[BLOCK_SIZE];
    read_blocks(compared_buffer, BOOT_SECTOR, 1);
    return (memcmp(compared_buffer, fs_signature, BLOCK_SIZE) != 0);
}

void dirtable_search(struct FAT32DirectoryEntry* entry, char* name, char* ext, uint32_t parent_cluster_number)
{
    read_clusters(&fat32_driver_state.dir_table_buf.table,parent_cluster_number,1);
    for (int i =0 ; i < CLUSTER_SIZE/sizeof(struct FAT32DirectoryEntry); i++){
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name,name,8) == 0 && memcmp(fat32_driver_state.dir_table_buf.table[i].ext,ext,3)==0){
            *entry =  fat32_driver_state.dir_table_buf.table[i];
        }
    }
    // assign entry with zero valued struct if not found 
    *entry = (struct FAT32DirectoryEntry) {0};
}

int8_t read(struct FAT32DriverRequest request)
{
    struct FAT32DirectoryEntry* entry;
    dirtable_search(entry, request.name,request.ext,request.parent_cluster_number);
    
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
    uint32_t cluster_number = ((uint32_t) entry->cluster_high) << 16 | (entry->cluster_low);
    if (cluster_number)
    {
        int itr = 0;
        // partition
        while(cluster_number!= FAT32_FAT_END_OF_FILE){
            read_clusters(request.buf + itr * CLUSTER_SIZE, cluster_number,1);
            itr++;
            cluster_number = fat32_driver_state.fat_table.cluster_map[cluster_number];
        }
        return 0;
    }

    // Unknown Errors
    return -1;

}

bool is_directory_empty(uint32_t parent_dir_cluster)
{
    // Get the directory table
    struct FAT32DirectoryTable *info;

    // Start the index at 2 because the first two entries are reserved
    int i = 2;
    read_clusters(info, parent_dir_cluster, 1);

    // iterate through all entris from 2 - 63, check if there is any entry that is not empty
    while (i < 64)
    {
        if (info->table[i].user_attribute == UATTR_NOT_EMPTY)
        {
            return false;
        }
        i++;
    }
    return true;
}


bool is_directory(uint32_t parent_cluster, uint32_t dir_cluster)
{
    // Check if dir_cluster is a directory
    struct FAT32DirectoryTable parent_directory_table;
    read_clusters(&parent_directory_table, parent_cluster, 1);
    bool flag = false;
    for (int i = 2; i < 64; i++)
    {
        if (parent_directory_table.table[i].attribute == ATTR_SUBDIRECTORY)
        {
            uint32_t current_cluster_checked = parent_directory_table.table[i].cluster_low | ((uint32_t) parent_directory_table.table[i].cluster_high) << 16;
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

uint8_t add_entry(const struct FAT32DriverRequest* request, uint32_t empty_cluster_number)
{
    read_clusters(&fat32_driver_state.dir_table_buf.table, request->parent_cluster_number, 1);

    uint8_t entry_idx = 2;
    bool found = false;
    while (entry_idx < 64 && !found)
    {
        if (fat32_driver_state.dir_table_buf.table[entry_idx].attribute == UATTR_NOT_EMPTY)
        {
            entry_idx++;
        }
        else
        {
            found = true;
        }
    }
    if (found)
    {
        fat32_driver_state.dir_table_buf.table[entry_idx] = (struct FAT32DirectoryEntry) {0};
        for (int i = 0; i < 8; i++)
        {
            fat32_driver_state.dir_table_buf.table[entry_idx].name[i] = request->name[i];
        }
        // Mark if directory
        if (request->buffer_size == 0)
        {
            fat32_driver_state.dir_table_buf.table[entry_idx].attribute = ATTR_SUBDIRECTORY;
        }
        // Add filesize if not directory
        else{
            fat32_driver_state.dir_table_buf.table[entry_idx].filesize = request->buffer_size;
        }
        fat32_driver_state.dir_table_buf.table[entry_idx].user_attribute = UATTR_NOT_EMPTY;
        for (int i = 0; i < 3; i++)
        {
            fat32_driver_state.dir_table_buf.table[entry_idx].ext[i] = request->ext[i];
        }
        fat32_driver_state.dir_table_buf.table[entry_idx].cluster_high = (uint16_t) (empty_cluster_number >> 16);
        fat32_driver_state.dir_table_buf.table[entry_idx].cluster_high = (uint16_t) empty_cluster_number;

        return 0;
    }
    else
    {
        return -1;
    }
}

int8_t write(struct FAT32DriverRequest request)
{
    // Check if parent directory exist
    if (!is_directory(2, request.parent_cluster_number))
    {
        return 2;
    }

    struct FAT32DirectoryEntry* entry;
    dirtable_search(entry, request.name, request.ext, request.parent_cluster_number);
    if (entry->user_attribute != UATTR_NOT_EMPTY)
    {
        return 1;
    }
    // Writing directory
    if (request.buffer_size == 0)
    {
        init_directory_table(&fat32_driver_state.dir_table_buf.table, request.name, request.parent_cluster_number);
        uint32_t cluster_number = fat32_driver_state.dir_table_buf.table[0].cluster_low | (((uint32_t) fat32_driver_state.dir_table_buf.table[0].cluster_high) << 16);
        uint8_t is_success = add_entry(&request, cluster_number);
        if (is_success == 0)
        {
            write_clusters(&fat32_driver_state.dir_table_buf.table, cluster_number, 1);
            read_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);
            fat32_driver_state.fat_table.cluster_map[cluster_number] = FAT32_FAT_END_OF_FILE;
            write_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);
            return 0;             
        }
        else
        {
            return -1;
        }
    }
    // Writing file
    else
    {
        uint32_t needed_cluster = ceil(request.buffer_size / (float) CLUSTER_SIZE);
        uint32_t count = 0;
        struct ClusterBuffer *buf_pointer = (struct ClusterBuffer*) request.buf;
        uint32_t parent_cluster_number = request.parent_cluster_number;
        uint32_t cluster_number;
        bool do_continue = true;
        read_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);
        while (count < needed_cluster && do_continue)
        {
            cluster_number = get_empty_cluster();
            if (cluster_number == -1)
            {
                do_continue = false;
            }
            else
            {
                uint8_t is_success = add_entry(&request, cluster_number);
                if (is_success == 0)
                {
                    fat32_driver_state.fat_table.cluster_map[parent_cluster_number] = cluster_number;
                    write_clusters(buf_pointer, cluster_number, 1);
                    parent_cluster_number = cluster_number;
                    count++;
                    buf_pointer++;
                }
                else
                {
                    do_continue = false;
                }
            }
        }
        // Last written partition is marked with EOF
        // Write updated FAT to bin
        fat32_driver_state.fat_table.cluster_map[parent_cluster_number] = FAT32_FAT_END_OF_FILE;
        write_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);
        if (do_continue)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    return -1;
}

void clear_buffer()
{
    memset(fat32_driver_state.cluster_buf.buf, 0, CLUSTER_SIZE);
}

bool is_available_entry(uint32_t parent_dir_cluster)
{
    struct FAT32DirectoryTable *info;
    bool found = false;
    int i = 2;
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

    return found;
}

uint32_t get_empty_cluster()
{
    bool found;
    uint16_t i = 3;
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
