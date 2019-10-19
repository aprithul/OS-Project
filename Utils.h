#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

char* strip(char* str);
char* make_singly_spaced(char* str);
char** split(const char* str_a, const char* del, int* length);
void free_split_text(char** split_text, int length);
int char_in_del(const char* del, char c);
void set_value_f(float* array, int count, float value);
void set_value_i(int* array, int count, int value);
char* read_file(char* file_name);
FILE* write_to_file(char* text, char* file_name, FILE *fp);
int is_text_whitespace(char* text);
int get_char_count(char c, char* str);
int sort_bubble(int arr_1[], int arr_2[], int len, int asc);
#endif
