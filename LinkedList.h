#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stdlib.h>
//#include "MetaData.h"
// Linked list declarations
//************************
// for type 'float'

typedef struct
{
    void* value;
}Node;

typedef struct
{
    Node** values;
    int* links;
    int count;
    int allocation_length;
    int start;
    int end;
}LinkedList;

void ll_construct(LinkedList* linked_list, int initial_count);
void ll_append(LinkedList* linked_list, Node* node);
int ll_insert_at(LinkedList* linked_list, int index, Node* node);
int ll_remove_at(LinkedList* linked_list, int index);
Node* ll_get(LinkedList* linked_list, int index);
void ll_remove(LinkedList* linked_list);

// end of linked list declaration for type 'float'
//***********************************************

#endif




