#ifndef _DIRTABLESTACK_H
#define _DIRTABLESTACK_H

#include "../../filesystem/fat32.h"
#include "../../shell/utils/shellutils.h"

struct DirTableStackElement
{
    uint32_t cluster_number;
    char name[8];
};

struct DirTableStack
{
    struct DirTableStackElement element[128];
    uint32_t idx_top;
};
/**
 * @brief Push element
 *
 * @param dts
 * @param table
 */
void push(struct DirTableStack *dts, struct FAT32DirectoryTable *table);
/**
 * @brief Pop element
 *
 * @param dts
 * @return true
 * @return false
 */
bool pop(struct DirTableStack *dts);
/**
 * @brief Peek element
 *
 * @param dts
 * @param table
 */
void peek(struct DirTableStack *dts, struct FAT32DirectoryTable *table);
/**
 * @brief print directory stack
 *
 * @param dts
 */
void get_full_path(struct DirTableStack *dts);
/**
 * @brief deep_copy_dirtable_stack
 *
 * @param dest
 * @param src
 */
void deep_copy_dirtable_stack(struct DirTableStack *dest, struct DirTableStack *src);
/**
 * @brief Get the actual cluster number. Return -1 means no path found, > 0 means succsess
 *
 * @param path
 * @param cwd_table
 * @return uint32_t
 */
int32_t get_actual_cluster_number(char *path, struct DirTableStack *dts);



#endif