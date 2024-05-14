
#include "../header/filesystem/fat32.h"

struct FAT32DriverState fat32_driver_state;

const uint8_t fs_signature[BLOCK_SIZE] = {
    'O',
    'p',
    'e',
    'r',
    'a',
    't',
    'i',
    'n',
    'g',
    ' ',
    'S',
    'y',
    's',
    't',
    'e',
    'm',
    'C',
    'r',
    'e',
    'a',
    't',
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
    ' ',
    'M',
    'e',
    'o',
    'n',
    'g',
    'm',
    'e',
    'o',
    'n',
    'g',
    ' ',
    '-',
    ' ',
    'K',
    'O',
    '2',
    'A',
    'm',
    'e',
    'l',
    ' ',
    ' ',
    '-',
    ' ',
    '1',
    '3',
    '5',
    '2',
    '2',
    '0',
    '4',
    '2',
    'A',
    'n',
    'g',
    'i',
    'e',
    ' ',
    '-',
    ' ',
    '1',
    '3',
    '5',
    '2',
    '2',
    '0',
    '4',
    '8',
    'N',
    'u',
    'e',
    'l',
    ' ',
    ' ',
    '-',
    ' ',
    '1',
    '3',
    '5',
    '2',
    '2',
    '0',
    '5',
    '8',
    'I',
    'a',
    'n',
    ' ',
    ' ',
    ' ',
    '-',
    ' ',
    '1',
    '3',
    '5',
    '2',
    '2',
    '0',
    '8',
    '0',
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
    '3',
    '3',
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
    '-',
    '2',
    '0',
    '2',
    '4',
    [BLOCK_SIZE - 2] = 'O',
    [BLOCK_SIZE - 1] = 'k',
};

uint32_t cluster_to_lba(uint32_t cluster)
{
    return BOOT_SECTOR + cluster * CLUSTER_BLOCK_COUNT;
}

void init_directory_table(struct FAT32DirectoryTable *dir_table, char *name, uint32_t parent_dir_cluster)
{
    // Initialize all entries to 0
    struct FAT32DirectoryEntry val = {0};

    // Set all entries to empty value
    for (size_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        dir_table->table[i] = val;
    }

    // Create entry 0 (for himself)
    // Important data = name, cluster high, clusterlow, attribute, user attribute, extension
    for (size_t i = 0; i < 8; i++)
    {
        dir_table->table[0].name[i] = name[i];
    }
    dir_table->table[0].attribute = ATTR_SUBDIRECTORY;
    dir_table->table[0].user_attribute = UATTR_NOT_EMPTY;

    if (parent_dir_cluster == ROOT_CLUSTER_NUMBER && memcmp(name, "root\0\0\0\0", 8) == 0)
    {
        // Special case for when creating root
        dir_table->table[0].cluster_low = (uint16_t)ROOT_CLUSTER_NUMBER;
        dir_table->table[0].cluster_high = (uint16_t)(ROOT_CLUSTER_NUMBER >> 16);
    }
    else
    {
        // Get the first empty cluster
        uint32_t res = get_empty_cluster();
        if (res != 0)
        {
            dir_table->table[0].cluster_low = (uint16_t)res;
            dir_table->table[0].cluster_high = (uint16_t)(res >> 16);
        }
    }

    // Case for root folder , init the root folder
    if (parent_dir_cluster == ROOT_CLUSTER_NUMBER && memcmp(name, "root\0\0\0\0", 8) == 0)
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

bool is_empty_storage(void)
{
    uint8_t compared_buffer[BLOCK_SIZE];
    read_blocks(compared_buffer, BOOT_SECTOR, 1);
    return (memcmp(compared_buffer, fs_signature, BLOCK_SIZE) != 0);
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

    // Init root folder
    init_directory_table(&fat32_driver_state.dir_table_buf, "root\0\0\0\0", ROOT_CLUSTER_NUMBER);
    write_clusters(&fat32_driver_state.dir_table_buf, ROOT_CLUSTER_NUMBER, 1);
    uint32_t cluster_number = fat32_driver_state.dir_table_buf.table[0].cluster_low | (((uint32_t)fat32_driver_state.dir_table_buf.table[0].cluster_high) << 16);
    fat32_driver_state.fat_table.cluster_map[cluster_number] = FAT32_FAT_END_OF_FILE;
    write_clusters(fat32_driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);
}

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
    struct FAT32DirectoryTable root_directory_table;
    read_clusters(&root_directory_table, ROOT_CLUSTER_NUMBER, 1);
}

void write_clusters(const void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    write_blocks(ptr, cluster_to_lba(cluster_number), cluster_count * CLUSTER_BLOCK_COUNT);
}

void read_clusters(void *ptr, uint32_t cluster_number, uint8_t cluster_count)
{
    read_blocks(ptr, cluster_to_lba(cluster_number), cluster_count * CLUSTER_BLOCK_COUNT);
}

/* -- CRUD Operation -- */

int8_t read_directory(struct FAT32DriverRequest request)
{
    struct FAT32DirectoryTable parent_dir_table;

    // tidak diketahui
    if (request.buffer_size != sizeof(parent_dir_table))
    {
        return -1;
    }

    read_clusters(&parent_dir_table, request.parent_cluster_number, 1);

    // format_directory_name(request.name, dir_entry.name);

    // char formatted_name[8];
    // // inisialisasi pakai spasi
    // memset(formatted_name, ' ', 8);
    // // copy nama dan tidak lebih dari 8 karaker
    // strncpy(formatted_name, request.name, 8);

    // Loop through the entries to find the directory, special case for root dir

    if (request.parent_cluster_number == ROOT_CLUSTER_NUMBER && memcmp(request.name, "root\0\0\0\0", 8) == 0 && memcmp(request.ext, "\0\0\0", 3) == 0)
    {
        // Read the root directory
        read_clusters(request.buf, ROOT_CLUSTER_NUMBER, 1);
        return 0;
    }

    for (uint32_t i = 2; i < (CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry)); i++)
    {
        struct FAT32DirectoryEntry *entry = &parent_dir_table.table[i];

        // Check if entry is the directory we are looking for
        if (memcmp(entry->name, request.name, 8) == 0 && memcmp(entry->ext, request.ext, 3) == 0)
        {
            // Found the directory

            // Check if it is actually a directory and not a file
            if ((entry->attribute != ATTR_SUBDIRECTORY))
            {
                return 1; // Not a directory
            }

            // Read the directory's contents into the buffer
            uint32_t dir_cluster = ((uint32_t)entry->cluster_high << 16) | entry->cluster_low;
            read_clusters(request.buf, dir_cluster, 1);
            // read_clusters(&fat32_driver_state.cluster_buf, dir_cluster, 1);

            return 0; // Success
        }
    }

    // tidak ditemukan
    return 2;
}

int8_t read(struct FAT32DriverRequest request)
{
    struct FAT32DirectoryEntry entry = {0};
    dirtable_search(&entry, request.name, request.ext, request.parent_cluster_number);

    // Not a file
    if (entry.attribute == ATTR_SUBDIRECTORY)
    {
        return 1;
    }
    // Not enough bffer
    if (request.buffer_size < entry.filesize)
    {
        return 2;
    }
    // Not found
    if (entry.user_attribute != UATTR_NOT_EMPTY)
    {
        return 3;
    }

    // Found
    uint32_t cluster_number = ((uint32_t)entry.cluster_high) << 16 | (entry.cluster_low);
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

int8_t write(struct FAT32DriverRequest request)
{
    // Check if parent directory exist

    if (!is_directory(ROOT_CLUSTER_NUMBER, request.parent_cluster_number))
    {
        return 2;
    }

    struct FAT32DirectoryEntry entry = {0};

    dirtable_search(&entry, request.name, request.ext, request.parent_cluster_number);

    // File or folder already exist
    if (entry.user_attribute == UATTR_NOT_EMPTY)
    {
        return 1;
    }
    // Writing directory
    if (request.buffer_size == 0)
    {
        // Make the directory table for himself
        struct FAT32DirectoryTable himself = {0};
        init_directory_table(&himself, request.name, request.parent_cluster_number);

        // Take the cluster number of the table
        uint32_t cluster_number = himself.table[0].cluster_low | (((uint32_t)himself.table[0].cluster_high) << 16);

        // Write the directory table to the disk
        uint8_t is_success = add_entry(&request, cluster_number);
        if (is_success == 0)
        {
            // write ulang parent
            write_clusters(&fat32_driver_state.dir_table_buf.table, request.parent_cluster_number, 1);
            // write FAT
            read_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);
            fat32_driver_state.fat_table.cluster_map[cluster_number] = FAT32_FAT_END_OF_FILE;
            write_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);
            // write diri dia sendiri
            write_clusters(&himself, cluster_number, 1);
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
        // Calculate the umber of cluster needed
        uint16_t needed_cluster = ceil(request.buffer_size / (float)CLUSTER_SIZE);

        // Init for itertator
        uint16_t count = 1;
        struct ClusterBuffer *buf_pointer = (struct ClusterBuffer *)request.buf;

        uint16_t cluster = get_empty_cluster();
        uint16_t next;

        uint8_t is_success = add_entry(&request, cluster);

        if (is_success != 0)
        {
            return -1;
        }
        else
        {

            write_clusters(&fat32_driver_state.dir_table_buf.table, request.parent_cluster_number, 1);
        }

        while (count <= needed_cluster)
        {
            fat32_driver_state.fat_table.cluster_map[cluster] = FAT32_FAT_END_OF_FILE;

            // Update the FAT table
            if (count != needed_cluster)
            {
                next = get_empty_cluster();
                fat32_driver_state.fat_table.cluster_map[cluster] = next;
            }
            // Write the buffer to the cluster
            write_clusters(buf_pointer, cluster, 1);

            write_clusters(&fat32_driver_state.fat_table.cluster_map, FAT_CLUSTER_NUMBER, 1);

            // Move to the next cluster
            cluster = next;
            count++;
            buf_pointer++;
        }
        return 0;

        // // Calculate the number of cluster needed
        // uint32_t needed_cluster = ceil(request.buffer_size / (float)CLUSTER_SIZE);

        // // Init for itertator
        // uint32_t count = 0;
        // // Init buffer
        // struct ClusterBuffer *buf_pointer = (struct ClusterBuffer *)request.buf;

        // // Save the parent cluster
        // uint32_t parent_cluster_number = request.parent_cluster_number;

        // uint32_t cluster_number;
        // bool do_continue = true;
        // read_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);
        // while (count < needed_cluster && do_continue)
        // {
        //     cluster_number = get_empty_cluster();
        //     if (cluster_number == 0)
        //     {
        //         do_continue = false;
        //     }
        //     else
        //     {

        //         uint8_t is_success = add_entry(&request, cluster_number);
        //         if (is_success == 0)
        //         {
        //             fat32_driver_state.fat_table.cluster_map[parent_cluster_number] = cluster_number;
        //             write_clusters(buf_pointer, cluster_number, 1);
        //             parent_cluster_number = cluster_number;
        //             count++;
        //             buf_pointer++;
        //         }
        //         else
        //         {
        //             do_continue = false;
        //         }
        //     }
        // }
        // // Last written partition is marked with EOF
        // // Write updated FAT to bin
        // fat32_driver_state.fat_table.cluster_map[parent_cluster_number] = FAT32_FAT_END_OF_FILE;
        // write_clusters(&fat32_driver_state.fat_table.cluster_map, 1, 1);
        // if (do_continue)
        // {
        //     return 0;
        // }
        // else
        // {
        //     return -1;
        // }
    }
    return -1;
}

int8_t delete(struct FAT32DriverRequest request)
{
    // Search for the entry in the desired parent directory
    struct FAT32DirectoryEntry entry = {0};
    dirtable_search(&entry, request.name, request.ext, request.parent_cluster_number);

    uint32_t actual_cluster_number = ((uint32_t)entry.cluster_high) << 16 | (entry.cluster_low);

    // Case Not Found
    if (entry.user_attribute != UATTR_NOT_EMPTY)
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
        for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
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
uint32_t get_empty_cluster()
{
    bool found = false;
    uint32_t i = 2;
    uint32_t idx;
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
        return 0;
    }
    else
    {
        return idx;
    }
}

uint8_t add_entry(const struct FAT32DriverRequest *request, uint32_t empty_cluster_number)
{
    // Read the parent directory table
    read_clusters(&fat32_driver_state.dir_table_buf.table, request->parent_cluster_number, 1);

    uint8_t entry_idx = 2;
    bool found = false;

    // Search for empty entry
    while (entry_idx < 64 && !found)
    {

        if (fat32_driver_state.dir_table_buf.table[entry_idx].user_attribute == UATTR_NOT_EMPTY)
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

        // Write the new entry to the Parent directory table
        fat32_driver_state.dir_table_buf.table[entry_idx] = (struct FAT32DirectoryEntry){0};
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
        else
        {
            fat32_driver_state.dir_table_buf.table[entry_idx].filesize = request->buffer_size;
        }
        fat32_driver_state.dir_table_buf.table[entry_idx].user_attribute = UATTR_NOT_EMPTY;
        for (int i = 0; i < 3; i++)
        {
            fat32_driver_state.dir_table_buf.table[entry_idx].ext[i] = request->ext[i];
        }
        fat32_driver_state.dir_table_buf.table[entry_idx].cluster_high = (uint16_t)(empty_cluster_number >> 16);
        fat32_driver_state.dir_table_buf.table[entry_idx].cluster_low = (uint16_t)empty_cluster_number;

        return 0;
    }
    else
    {
        return 64;
    }
}

// Check whether or not a file or folder with the same name and extention exist in the parent directory
void dirtable_search(struct FAT32DirectoryEntry *entry, char *name, char *ext, uint32_t parent_cluster_number)
{

    // This assumes that no file have an extention of \0\0\0 so folder and file will definitely be diffrentiated by its extention
    read_clusters(&fat32_driver_state.dir_table_buf.table, parent_cluster_number, 1);

    for (uint32_t i = 2; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
    {
        if (memcmp(fat32_driver_state.dir_table_buf.table[i].name, name, 8) == 0 && memcmp(fat32_driver_state.dir_table_buf.table[i].ext, ext, 3) == 0)
        {
            // If found return the entry
            *entry = fat32_driver_state.dir_table_buf.table[i];
            return;
        }
    }
    // assign entry with zero valued struct if not found

    // *entry = (struct FAT32DirectoryEntry){0};
}

// when first called, we will call root as it parent_cluster
bool is_directory(uint32_t parent_cluster, uint32_t dir_cluster)
{
    struct FAT32DirectoryTable parent_directory_table;
    read_clusters(&parent_directory_table, parent_cluster, 1);
    bool flag = false;

    // iterate through all possible entry in root
    for (uint32_t i = 0; i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry); i++)
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
                if (i != 0 && i != 1)
                {
                    flag = is_directory(current_cluster_checked, dir_cluster);
                }
                if (flag)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool is_directory_empty(uint32_t dir_cluster)
{
    // Get the directory table

    // Start the index at 2 because the first two entries are reserved
    uint32_t i = 2;
    read_clusters(&fat32_driver_state.dir_table_buf, dir_cluster, 1);

    // iterate through all entris from 2 - 63, check if there is any entry that is not empty
    while (i < CLUSTER_SIZE / sizeof(struct FAT32DirectoryEntry))
    {
        if (fat32_driver_state.dir_table_buf.table[i].user_attribute == UATTR_NOT_EMPTY)
        {
            return false;
        }
        i++;
    }
    return true;
}

void clear_buffer()
{
    memset(fat32_driver_state.cluster_buf.buf, 0, CLUSTER_SIZE);
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

uint32_t ceil(float num)
{
    uint32_t int_num = (uint32_t)num;
    if (num > int_num)
        return int_num + 1;
    else
        return int_num;
}