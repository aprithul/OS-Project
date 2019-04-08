#include "IO.h"
#include <stdio.h>
#include <string.h>
#include "Resource.h"


void* hdd_input_thread(void* i_args)
{
    sem_wait(&(resources.hdd_sem)); // sempahore wait
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 
    sem_post(&(resources.hdd_sem)); // semaphore post
    return NULL;
}

void* keyboard_input_thread(void* i_args)
{
    pthread_mutex_lock(&(resources.keyboard_mutex)); //mutex lock 
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 
    pthread_mutex_unlock(&(resources.keyboard_mutex)); // mutex unlock
    return NULL;
}

void* scanner_input_thread(void* i_args)
{
    pthread_mutex_lock(&(resources.scanner_mutex)); // mutex lock
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 
    pthread_mutex_unlock(&(resources.scanner_mutex)); //mutex unlock
    return NULL;
}


void* hdd_output_thread(void* i_args)
{
    sem_wait(&(resources.hdd_sem)); // sempahore wait
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 
    sem_post(&(resources.hdd_sem)); // sempahore post
    return NULL;
}

void* monitor_output_thread(void* i_args)
{
    pthread_mutex_lock(&(resources.monitor_mutex)); // mutex lock
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 
    pthread_mutex_unlock(&(resources.monitor_mutex)); // mutex unlock
    return NULL;
}

void* projector_output_thread(void* i_args)
{
    sem_wait(&(resources.projector_sem)); // sempahore wait
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 
    sem_post(&(resources.projector_sem)); // sempahore wait
    return NULL;
}

pthread_t start_input_thread(TimerArgs* t_args, char* resource_name)
{
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(!strcmp(resource_name, "hard drive"))
    {
        pthread_create(&t_id, &attr, hdd_input_thread,t_args);
    }
    else if(!strcmp(resource_name, "keyboard"))
    {
        pthread_create(&t_id, &attr, keyboard_input_thread,t_args);
    }
    else if(!strcmp(resource_name, "scanner"))
    {
        pthread_create(&t_id, &attr, scanner_input_thread,t_args);
    }

    return t_id;
}

pthread_t start_output_thread(TimerArgs* t_args, char* resource_name)
{
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    if(!strcmp(resource_name, "hard drive"))
    {
        pthread_create(&t_id, &attr, hdd_output_thread,t_args);
    }
    else if(!strcmp(resource_name,"monitor"))
    {
        pthread_create(&t_id, &attr, monitor_output_thread,t_args);
    }
    else if(!strcmp(resource_name, "projector"))
    {
        pthread_create(&t_id, &attr, projector_output_thread,t_args);
    }

    return t_id;
}
