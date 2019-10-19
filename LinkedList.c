#include "LinkedList.h"
#include "Utils.h"

// constructor. start with initial count number of items
// since there is no templates in c, the list items have type 'Node*'
void ll_construct(LinkedList* linked_list, int initial_count)
{
    linked_list->values = (Node**)malloc(sizeof(Node*)*initial_count);
    linked_list->links =  (int*)malloc(sizeof(int)*initial_count);
    set_value_i(linked_list->links, initial_count, -1);
    linked_list->start = -1;
    linked_list->end   = -1;
    linked_list->count = 0;
    linked_list->allocation_length = initial_count; 
}

// appending doesn't allocate dynamic memory right now, in to-do.
// so will fail if count exceeds initial_count as specified in constructor
void ll_append(LinkedList* linked_list, Node* value)
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


int ll_insert_at(LinkedList* linked_list, int index, Node* value)
{
    if(index<0 || index>linked_list->count)
        return -1;
     // find empty position
    int _i = 0;
    for( ; _i<linked_list->allocation_length; _i++)
    {
        if(linked_list->links[_i] == -1)
            break;
    }
    
    if(_i == linked_list->allocation_length) // not enough room in list to store value
        return -1;
    linked_list->values[_i] = value;
     
    if(index == 0)// by definition , index 0 means linked_list->start 
    {
        if(linked_list->count == 1)
        {
            int prev_start = linked_list->start;
            linked_list->links[_i] = prev_start;
            linked_list->start = _i;
            linked_list->end = prev_start; 
        }
        else
        {
            linked_list->links[_i] = linked_list->start;
            linked_list->start = _i;
        }
        linked_list->count++;
        return 1; 
     }

    int _j = linked_list->start;
    int _cur_index = 0; 
    while(1)
    {
        if(_cur_index+1 == index)
        {
            linked_list->links[_i] = linked_list->links[_j]; 
            linked_list->links[_j] = _i;
            if(linked_list->end == _j)
                linked_list->end = _i; 
            linked_list->count++;
            return 1; 
        }
        _cur_index++; 
        if(_j == linked_list->end)
            break;
         _j = linked_list->links[_j];
    }
}

//remove item
// returns {1 if item removed} {-1 if not removed} (index doesn't exist)
int ll_remove_at(LinkedList* linked_list, int index)
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
            linked_list->end = -1;
        }
        else
        {
            int _target = linked_list->links[linked_list->start];
            linked_list->links[linked_list->start] = -1;
            linked_list->start = _target;
            //printf( "previous target %d\n", linked_list->start);
//            printf("ll:  %d, ", *((int*)(ll_get((linked_list),0)->value)));
 //           printf("removed"); 
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

// get the Node at index position
Node* ll_get(LinkedList* linked_list, int index)
{
    int _i = linked_list->start;
    int _cur_index = 0; 
    while(1)
    {
       // printf("%d \n", _cur_index);
        if(_cur_index == index)
        {
            return linked_list->values[_i];
        }
        _cur_index++; 
        if(_i == linked_list->end)
            break;
         _i = linked_list->links[_i];
    }
}

