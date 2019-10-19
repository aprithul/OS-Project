#include "Memory.h"
#include "Configuration.h"
#include <pthread.h>
#include <stdlib.h>
extern Configuration config;
void allocate_memory(TimerArgs* t_args,unsigned int* address, int size) // not taking size into account at the moment
{
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    *address = rand()%config.allocated_memory_size; 
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


