
#include "Utils.h"
#include <stdlib.h>

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


char* read_file(char* file_name)
{
    int n = 10;
    char* buffer = (char*)malloc(n*sizeof(char));
    FILE *fp;
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
