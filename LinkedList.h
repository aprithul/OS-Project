#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stdlib.h>
#include "Utils.h"

// Linked list declarations
//************************
// for type 'float'

typedef struct
{
    float* values;
    int* links;
    int count;
    int allocation_length;
    int start;
    int end;
}LinkedListf;

void llf_construct(LinkedListf* linked_list, int initial_count)
{
    linked_list->values = (float*)malloc(sizeof(float)*initial_count);
    linked_list->links =  (int*)malloc(sizeof(int)*initial_count); 
    set_value_i(linked_list->links, initial_count, -1);
    linked_list->start = -1;
    linked_list->end   = -1;
    linked_list->count = 0;
    linked_list->allocation_length = initial_count; 
}

void llf_append(LinkedListf* linked_list, float value)
{
    // find empty position
    int _i = 0;
    for( ; _i<linked_list->allocation_length; _i++)
    {
        if(linked_list->links[_i] == -1)
            break;
    }

    if(_i == linked_list->allocation_length)
    {
        // no empty position found, need to reconstruct list    
    }
   
    // put value at _i, and adjust end

    if(linked_list->count == 0)
    {
        linked_list->start = _i;
    }
    else 
    {
        linked_list->links [linked_list->end] = _i;
    }
   
    linked_list->end = _i;
    linked_list->values[_i] = value;
    linked_list->links [_i] = linked_list->allocation_length; // 'hack' to put a positive value that's not being used by anyone else 
    linked_list->count++; 
}

void llf_insert_at(LinkedListf* linked_list, int index, float value)
{
    if(index<0 || index>count)
        return;
     // find empty position
    int _i = 0;
    for( ; _i<linked_list->allocation_length; _i++)
    {
        if(linked_list->links[_i] == -1)
            break;
    }
    
    if(index == 0)// by definition , index 0 means linked_list->start 
    {
        if(linked_list->count == 1)
        {
            linked_list->links[linked_list->start] = -1;
            linked_list->start = -1;
        }
        else
        {
            int _target = linked_list->links[linked_list->start];
            linked_list->links[linked_list->start] = -1;
            linked_list->start = _target;
        }
        linked_list->count--;
        return 1; 
     }

    _i = linked_list->start;
    int _cur_index = 0; 
    while(1)
    {
       // printf("%d \n", _cur_index);
        if(_cur_index+1 == index)
        {
            int _i_next =  linked_list->links[linked_list->links[_i]];
            if(linked_list->end == linked_list->links[_i])
                linked_list->end = _i;
            linked_list->links[_i] = _i_next;
            linked_list->count--;
            return 1; 
        }
        _cur_index++; 
        if(_i == linked_list->end)
            break;
         _i = linked_list->links[_i];
    }


    

}

//remove item
// returns {1 if item removed} {-1 if not removed}
int llf_remove_at(LinkedListf* linked_list, int index)
{
    if(index<0 || index>=linked_list->count)
    {
       return -1; 
    }
    int _i = 0;
     
    // find the node pointing to index
    // handle removing starting node separately 
    if(index == 0)// by definition , index 0 means linked_list->start 
    {
        if(linked_list->count == 1)
        {
            linked_list->links[linked_list->start] = -1;
            linked_list->start = -1;
        }
        else
        {
            int _target = linked_list->links[linked_list->start];
            linked_list->links[linked_list->start] = -1;
            linked_list->start = _target;
        }
        linked_list->count--;
        return 1; 
     }

    _i = linked_list->start;
    int _cur_index = 0; 
    while(1)
    {
       // printf("%d \n", _cur_index);
        if(_cur_index+1 == index)
        {
            int _i_next =  linked_list->links[linked_list->links[_i]];
            if(linked_list->end == linked_list->links[_i])
                linked_list->end = _i;
            linked_list->links[_i] = _i_next;
            linked_list->count--;
            return 1; 
        }
        _cur_index++; 
        if(_i == linked_list->end)
            break;
         _i = linked_list->links[_i];
    }
 
    return -1; 
}


// print linked list [will change]
void llf_print(LinkedListf* linked_list)
{
    int _i = linked_list->start;
    while(_i>=0)
    {
        float value = linked_list->values[_i];
        printf("%d \n", (int)value);
        if(_i == linked_list->end)
            break;
         _i = linked_list->links[_i];
        
    }

}

// end of linked list declaration for type 'float'
//***********************************************

#endif




