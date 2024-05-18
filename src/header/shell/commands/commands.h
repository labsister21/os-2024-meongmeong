#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "../../filesystem/fat32.h"
#include "../utils/shellutils.h"
#include "../../stdlib/string-lib.h"
#include "../../shell/datastructure/dirtablestack.h"

void execute_commands(char *buffer, struct DirTableStack *dts);

void cd(char *dir, struct DirTableStack *dts);

void ls(struct DirTableStack *dts);

void mkdir(char *dir, struct DirTableStack *dts);

void cat(char *path, struct DirTableStack *dts);

bool cp(char *src_path, char *dest_path, struct DirTableStack *dts);

void rm(char *path, struct DirTableStack *dts);

void mv(char *src_path, char *dest_path, struct DirTableStack *dts);

void find(char *filename);

void help();

void clear();

void find_helper(char *name, char *ext, struct DirTableStack *dts);

void exec(char* filename, struct DirTableStack* dts);

void ps();

#endif