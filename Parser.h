#ifndef PARSER_H
#define PARSER_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


char* strip(char* str);
char* make_singly_spaced(char* str);
char** split(const char* str_a, const char del, int* length);
void free_split_text(char** split_text, int length);

#endif
