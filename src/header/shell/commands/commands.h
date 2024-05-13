#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "../../filesystem/fat32.h"
#include "../utils/shellutils.h"
#include "../../stdlib/string.h"

void execute_commands(char* buffer);

void cd(char *dir);

void ls();

void mkdir(char *dir);

void cat(char *filename, char *ext);

void cp(char *filename);

void rm(char *filename, char *ext);

void mv(char *filename, char *dirname);

void find(char *filename);

#endif