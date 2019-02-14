#ifndef PARSER_H
#define PARSER_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* lstrip(char* str)
{
    int len = strlen(str);
    //printf("%d \n", len);
    int _i = 0; 
    while(str[0] == ' ')
    {
        //printf("%c \n", str[0]);
        str++; 
    }
    return str;

}

char* rstrip(char* str)
{
    int len = strlen(str);
    char* str_cpy = malloc( sizeof(char)*len);
    memcpy(str_cpy, str, len);
    char* _str = str_cpy+len-1;
    while( *_str == ' ' && len>0)
    {
        len--;
        _str--;
    }
    if(len>0)
    {
        *(_str+1) = '\0';
    }
    return str_cpy;
}


char** split_string(const char* str_a, const char del, int* length)
{
    char** tokens = malloc(sizeof(char*));
    
    int _i = 0;
    int start = 0;
    *length = 0; 
    while(str_a[_i] != '\0')
    {
        if(str_a[_i] == del)
        {
            if(_i-start > 0)
            {
                tokens[*length] = malloc( (sizeof(char)*(_i-start))+1);
                memcpy(tokens[*length], &str_a[start], _i-start);
                //tokens[*length][_i-start] = '\0';
                (*length)++;

                tokens = realloc(tokens, sizeof(char*)*((*length)+1));
            }
            while(str_a[_i] == del)
                _i++;
            start = _i;
            _i--; 
        } 
        _i++;
    }    
    if(_i-start > 0)
    { 
        tokens[*length] = malloc( (sizeof(char)*(_i-start))+1);
        memcpy(tokens[*length], &str_a[start], _i-start);
        (*length)++; 
    }
 
    //char* str_b;
    ///strcpy(str_b,str_a);
    //make a local copy of the string, since original string will get modified
    //char** tok = strtok(str_a, " "); 
     
    return tokens; 
}


#endif
