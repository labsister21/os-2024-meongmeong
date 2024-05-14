#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "../../filesystem/fat32.h"
#include "../utils/shellutils.h"
#include "../../stdlib/string-lib.h"
#include "../../shell/datastructure/dirtablestack.h"

void execute_commands(char *buffer);

void cd(char *dir, struct DirTableStack *dts);

void ls(struct FAT32DirectoryTable *cwd_table);

void mkdir(char *dir, struct FAT32DirectoryTable *cwd_table);

void cat(char *name, char *ext, struct FAT32DirectoryTable cwd_table);

void cp(char *filename);

void rm(char *name, char *ext, struct FAT32DirectoryTable *cwd_table);

void mv(char *filename, char *dirname);

void find(char *filename);

#endif