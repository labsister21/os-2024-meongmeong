#ifndef __STRING__H
#define __STRING__H

#include <bits/stdint-uintn.h> // unsigned int handling
#include <bits/stdint-intn.h> // int handling
#include <stdbool.h> // boolean handling
#include <stddef.h>

/***
 * mengembalikan panjang
 */
size_t strlen(char *str);

int strcmp(char *str1, char *str2);

void strcpy(char *dst, char *src);

int strparse(char *input, char output[12][128], char *delim);

char *strncpy(char *dest, const char *src, size_t n);

char *strtok(char *str, const char *delim);

char* strcat(char* dest, const char* src);

#endif