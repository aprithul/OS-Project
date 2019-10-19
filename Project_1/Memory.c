#include "Memory.h"
#include "Configuration.h"
#include <pthread.h>
#include <stdlib.h>
extern Configuration config;
unsigned int memory_allocation_pointer;

void allocate_memory(TimerArgs* t_args,unsigned int* address, int size) // not taking size into account at the moment
{
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    if(memory_allocation_pointer+config.memory_block_size > config. allocated_memory_size)
        memory_allocation_pointer = 0x0;

    *address = memory_allocation_pointer;//rand()%config.allocated_memory_size; 
    memory_allocation_pointer += config.memory_block_size;
     
    pthread_create(&t_id, &attr, timer_countdown,t_args); 
    pthread_join(t_id, NULL); 
}

void block_memory(TimerArgs* t_args) // not taking size into account at the moment
{
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&t_id, &attr, timer_countdown,t_args); 
    pthread_join(t_id, NULL); 
}


