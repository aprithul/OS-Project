#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

 
void set_value_f(float* array, int count, float value);
void set_value_i(int* array, int count, int value);
char* read_file(char* file_name);
FILE* write_to_file(char* text, char* file_name, FILE *fp);
int is_text_whitespace(char* text);

#endif
