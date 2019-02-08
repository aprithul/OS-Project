#include <stdio.h>
#include "LinkedList.h"

int main(int argc, char *argv[])
{
    LinkedListf linked_list;
    llf_construct(&linked_list, 10);

    llf_append(&linked_list, 1);
    llf_append(&linked_list, 2);
    llf_append(&linked_list, 3);
    llf_append(&linked_list, 4);
    llf_append(&linked_list, 5);
    llf_append(&linked_list, 6);
    llf_append(&linked_list, 7);
    llf_append(&linked_list, 8);
    llf_append(&linked_list, 9);
    llf_append(&linked_list, 10);
   /* llf_remove_at(&linked_list, 0);  
    llf_remove_at(&linked_list, 0);
    
    llf_insert_at(&linked_list, 0, 33);
    llf_append(&linked_list, 91);
    llf_insert_at(&linked_list, 2, 53);*/
    printf("value at 6 : %f \n", llf_get(&linked_list, 6));
    llf_print(&linked_list); 
    }
