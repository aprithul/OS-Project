#include "IO.h"
#include <stdio.h>

void* input_thread(void* i_args)
{
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 
    return NULL;
}

void* output_thread(void* i_args)
{
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 
    return NULL;
}

pthread_t start_input_thread(TimerArgs* t_args, PCB* pcb)
{
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&t_id, &attr, input_thread,t_args);
    return t_id;
}

pthread_t start_output_thread(TimerArgs* t_args, PCB* pcb)
{
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&t_id, &attr, output_thread,t_args);
    return t_id;
}
