#include "Utils.h"
#include <stdlib.h>


FILE *fp;

void set_value_f(float* array, int count, float value)
{
    while((--count) >= 0)
    {
        array[count] = value;
    } 
}
void set_value_i(int* array, int count, int value)
{
    while((--count) >= 0)
    {
        array[count] = value;
    } 
}

FILE* write_to_file(char* text, char* file_name, FILE *fp)
{
    if(fp == NULL)
    {
        fp = fopen(file_name, "a");
    }
    fputs(text, fp); 
    return fp;
}

char* read_file(char* file_name)
{
    int n = 1024;
    char* buffer = (char*)malloc(n*sizeof(char));
    int read_char_count = 0;
    if(fp = fopen(file_name, "r"))
    {
        int ch;
        while(ch = fgetc(fp))
        {
            if(ch == EOF)
                break;
            read_char_count++;
            if(read_char_count > n)
            {
                n*=2;
                buffer = realloc(buffer, sizeof(char)*n);
            }
            *(buffer+read_char_count-1) = ch; 
        }
        fclose(fp); 
    }
    else
        buffer = NULL;
    
    return buffer;
}
