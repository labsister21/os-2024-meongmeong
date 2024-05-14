#include "../header/shell/datastructure/dirtablestack.h"

void push(struct DirTableStack *dts, struct FAT32DirectoryTable *dirtable)
{
    dts->idx_top++;
    dts->element[dts->idx_top].cluster_number = dirtable->table[0].cluster_low | ((uint32_t)dirtable->table[0].cluster_high) << 16;
    memcpy(dts->element[dts->idx_top].name, dirtable->table[0].name, 8);
}

bool pop(struct DirTableStack *dts)
{
    if (dts->idx_top > 0)
    {
        dts->idx_top--;
        return true;
    }
    else
    {
        return false;
    }
}

void peek(struct DirTableStack *dts, struct FAT32DirectoryTable *dirtable)
{
    uint32_t current_cluster_number = dts->element[dts->idx_top].cluster_number;
    struct FAT32DriverRequest req;
    make_request(&req, dirtable, sizeof(struct FAT32DirectoryTable), current_cluster_number, dts->element[dts->idx_top].name, "\0\0\0");
    int32_t retcode;
    sys_read_dir(&req);
}

void get_full_path(struct DirTableStack *dts)
{
    for (uint32_t i = 0; i <= dts->idx_top; i++)
    {
        shell_put(dts->element[i].name, BIOS_WHITE);
        shell_put("/", BIOS_WHITE);
    }
    shell_put("\n", BIOS_WHITE);
}
void deep_copy_dirtable_stack(struct DirTableStack *dest, struct DirTableStack *src)
{
    // Copy the top index
    dest->idx_top = src->idx_top;

    // Copy each element of the element array
    for (uint32_t i = 0; i <= src->idx_top; i++)
    {
        // Copy cluster number
        dest->element[i].cluster_number = src->element[i].cluster_number;
        // Copy name
        memcpy(dest->element[i].name, src->element[i].name, 8);
    }
}
int32_t get_actual_cluster_number(char *path, struct DirTableStack *dts)
{
    struct FAT32DirectoryTable *cwd_table;

    struct FAT32DriverRequest req;
    struct DirTableStack *dts_copy;
    deep_copy_dirtable_stack(dts_copy, dts);
    peek(dts_copy, cwd_table);

    char paths[12][128];

    uint8_t path_num = strparse(path, paths, "/");

    // check tiap token
    char name[9];
    char ext[4];

    for (uint8_t i = 0; i < path_num; i++)
    {
        memset(name, '\0', 9);
        memset(ext, '\0', 4);
        parse_file_name(paths[i], name, ext);
        // Kalo titik dua naik
        if (memcmp(paths[i], "..", strlen(paths[i])) == 0)
        {
            pop(dts_copy);
        }
        // Kalo titik satu skip
        else if (memcmp(paths[i], ".", strlen(paths[i])) == 0)
        {
            continue;
        }
        // Kalo dia file, tapi bukan end of dir
        else if (memcmp(ext, 0, 3) != 0)
        {
            if (i != path_num - 1)
            {
                return -1;
            }
            else
            {
                uint32_t current_cluster_number = cwd_table->table[0].cluster_low | ((uint32_t)cwd_table->table[0].cluster_high) << 16;
                struct FAT32DriverRequest req;
                void *buffer;
                make_request(&req, cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, name, ext);
                int32_t retcode = sys_read_file(&req);
                if (retcode != 0)
                {
                    return retcode;
                }
                return dts_copy->element[dts_copy->idx_top].cluster_number;
            }
        }
        // Kalo dia directory, maka push ke curr directory
        else
        {
            uint32_t current_cluster_number = cwd_table->table[0].cluster_low | ((uint32_t)cwd_table->table[0].cluster_high) << 16;
            struct FAT32DriverRequest req;
            make_request(&req, cwd_table, sizeof(struct FAT32DirectoryTable), current_cluster_number, paths[i], "\0\0\0");
            int32_t retcode = sys_read_dir(&req);
            if (retcode != 0)
            {
                return retcode;
            }
            push(dts_copy, cwd_table);
        }
    }

    return dts_copy->element[dts_copy->idx_top].cluster_number;
}
