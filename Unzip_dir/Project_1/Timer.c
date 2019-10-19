#include "Timer.h"
#include <stdio.h>
unsigned long get_current_time()
{
    struct timeval tp;
    int code = gettimeofday(&tp,0);
    unsigned long current_time = tp.tv_sec * (int)1e6 + tp.tv_usec; 
    return current_time;
}

void set_timer(TimerArgs* t_args)
{
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
   
    pthread_create(&t_id, &attr, timer_countdown,t_args);  
    pthread_join(t_id, NULL); 

}

void* timer_countdown(void* args)
{
    unsigned long target_time = get_current_time() + ((TimerArgs*)args)->duration;
    while( get_current_time() < target_time)
       ; //printf("%lu , %lu\n", get_current_time(), target_time);
    if( ((TimerArgs*)args)->callback)
        ((TimerArgs*)args)->callback();
}

double mic_to_sec(unsigned long micro_sec)
{
    return (micro_sec/(double)(1e6));
}
