#ifndef UTILS_H
#define UTILS_H

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


#endif
