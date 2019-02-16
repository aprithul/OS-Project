#include "Parser.h"

char* strip(char* str)
{
    int len = strlen(str);
    int _i = 0; 
    while(str[0] == ' ')
    {
        str++; 
    }
    
    len = strlen(str);
    char* _str = str+len-1;
    while( *_str == ' ' && len>0)
    {
        len--;
        _str--;
    }
    if(len>0)
    {
        *(_str+1) = '\0';
    }

    return str;
}

char* make_singly_spaced(char* str)
{
    int len = strlen(str);
    char* buffer = (char*)malloc(sizeof(char)*len);
    int _i = 0;
    int _j = 0;
    int flag = 0;
    for( ; _i<=len; _i++)
    {
        if(str[_i] != ' ')
        {
            flag = 0;
            buffer[_j] = str[_i];
            _j++;
        }
        else if(!flag)
        {
            flag = 1;
            buffer[_j] = str[_i];
            _j++;
        } 
    } 
    memcpy(str,buffer,_j);
    free(buffer); 
    return str;
}

char** split(const char* str_a, const char del, int* length)
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
                tokens[*length][_i-start] = '\0';
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

void free_split_text(char** split_text, int length)
{
    for(;length>0;length--)
    {
        free(split_text[length-1]);
    }
    free(split_text);
}
