#include <stdio.h>
#include "LinkedList.h"

int main(int argc, char *argv[])
{
    LinkedListf linked_list;
    llf_construct(&linked_list, 10);

    llf_append(&linked_list, 5);
    llf_append(&linked_list, 4);
    llf_append(&linked_list, 1);
    llf_append(&linked_list, -50);
    llf_append(&linked_list, 10);
    
 
    
    llf_remove_at(&linked_list, -1);  
    llf_remove_at(&linked_list, 2);
    llf_append(&linked_list, 200);  
    llf_remove_at(&linked_list, 0);
    llf_remove_at(&linked_list, 1);
    llf_remove_at(&linked_list, 0);
    llf_print(&linked_list); 
    }
