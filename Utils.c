#include "Utils.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


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

/*
char* read_file(char* file_name)
{
FILE *file;
size_t nread;
char* buf = (char*)malloc(1024*sizeof(char));
file = fopen(file_name, "r");
if (file) {
    while ((nread = fread(buf, 1, sizeof buf, file)) > 0)
        fwrite(buf, 1, nread, stdout);
    if (ferror(file)) {
        printf("Some error");
    }
    fclose(file);
}
printf("%s :read", buf);
return buf;
}*/
char* read_file(char* file_name)
{
    int n = 1024;
    FILE *fp;
    char* buffer = (char*)malloc(n*sizeof(char));
    int read_char_count = 0;
    fp = fopen(file_name, "r");
    
    if(fp!=NULL)
    {
        int ch;
        while((ch = fgetc(fp)) != EOF)
        {
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
   
    *(buffer+read_char_count) = '\0'; 
//    printf("%d %s\n",(int)buffer[0], file_name ); 
    return buffer;
}

int is_text_whitespace(char* text)
{
//    printf("%d %s\n", (int)strlen(text), text);
    //printf("%s this\n",text);
    while (*text != '\0') 
    {
        if (!isspace(*text))
        {
            
 //           printf("%c hello\n", *text);
            return 0;
        }
        text++;
    }
    return 1;

}

