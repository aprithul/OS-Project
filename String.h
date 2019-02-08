#ifndef STRING_HPP
#define STRING_HPP

struct String
{
    char* value;
    int (*get_string_length)(char* string);
};


int get_string_length(char* string)
{
    int length = 0;
    int _i = 0;
    while(string[_i] != '\0')
    {
        length++;
        _i++;
    }
    return length;
}


void cosntruct_string(struct String* string)
{
    (*string).get_string_length = get_string_length;
}



#endif
