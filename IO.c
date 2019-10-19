#include "IO.h"
#include <stdio.h>
#include <string.h>
#include "Resource.h"
#include "Logger.h"
#include "PCB.h"
extern double simulator_start_time;
extern int simulator_complete;

char message_buffer[4096]; 
void* hdd_input_thread(void* i_args)
{

    simulator_complete = 0; 
    sem_wait(&(resources.hdd_sem)); // sempahore wait
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 
    
    sprintf(message_buffer, "%f - Process %d: end %s input", mic_to_sec(get_current_time() - simulator_start_time), ((TimerArgs*)(i_args))->pcb->pid,  ((TimerArgs*)(i_args))->resource_name);
    ((TimerArgs*)(i_args))->pcb->process_state = RUNNING;
    
    log_with_config(message_buffer);
    sem_post(&(resources.hdd_sem)); // semaphore post
    simulator_complete = 1; 
    return NULL;
}

void* keyboard_input_thread(void* i_args)
{
    simulator_complete = 0; 
    pthread_mutex_lock(&(resources.keyboard_mutex)); //mutex lock 
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 

    sprintf(message_buffer, "%f - Process %d: end %s input", mic_to_sec(get_current_time() - simulator_start_time), ((TimerArgs*)(i_args))->pcb->pid,  ((TimerArgs*)(i_args))->resource_name);
    ((TimerArgs*)(i_args))->pcb->process_state = RUNNING;

    log_with_config(message_buffer);
    pthread_mutex_unlock(&(resources.keyboard_mutex)); // mutex unlock
    simulator_complete = 1; 
    return NULL;
}

void* scanner_input_thread(void* i_args)
{
    simulator_complete = 0; 
    pthread_mutex_lock(&(resources.scanner_mutex)); // mutex lock
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 

    sprintf(message_buffer, "%f - Process %d: end %s input", mic_to_sec(get_current_time() - simulator_start_time), ((TimerArgs*)(i_args))->pcb->pid,  ((TimerArgs*)(i_args))->resource_name);
    ((TimerArgs*)(i_args))->pcb->process_state = RUNNING;
    
    log_with_config(message_buffer);
    pthread_mutex_unlock(&(resources.scanner_mutex)); //mutex unlock
    simulator_complete = 1; 
    return NULL;
}


void* hdd_output_thread(void* i_args)
{
    simulator_complete = 0; 
    sem_wait(&(resources.hdd_sem)); // sempahore wait
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL);

    sprintf(message_buffer, "%f - Process %d: end %s output", mic_to_sec(get_current_time() - simulator_start_time), ((TimerArgs*)(i_args))->pcb->pid,  ((TimerArgs*)(i_args))->resource_name);
    ((TimerArgs*)(i_args))->pcb->process_state = RUNNING;
    
    log_with_config(message_buffer);
    sem_post(&(resources.hdd_sem)); // sempahore post
    simulator_complete = 1;
    return NULL;
}

void* monitor_output_thread(void* i_args)
{
    simulator_complete = 0; 
    pthread_mutex_lock(&(resources.monitor_mutex)); // mutex lock
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 

    sprintf(message_buffer, "%f - Process %d: end %s output", mic_to_sec(get_current_time() - simulator_start_time), ((TimerArgs*)(i_args))->pcb->pid,  ((TimerArgs*)(i_args))->resource_name);
    ((TimerArgs*)(i_args))->pcb->process_state = RUNNING;
    
    log_with_config(message_buffer);
    pthread_mutex_unlock(&(resources.monitor_mutex)); // mutex unlock
    simulator_complete = 1;
    return NULL;
}

void* projector_output_thread(void* i_args)
{
    simulator_complete = 0; 
    sem_wait(&(resources.projector_sem)); // sempahore wait
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    
    pthread_create(&t_id, &attr, timer_countdown,i_args); 
    pthread_join(t_id, NULL); 

    sprintf(message_buffer, "%f - Process %d: end %s output", mic_to_sec(get_current_time() - simulator_start_time), ((TimerArgs*)(i_args))->pcb->pid,  ((TimerArgs*)(i_args))->resource_name);
    ((TimerArgs*)(i_args))->pcb->process_state = RUNNING;
    
    log_with_config(message_buffer);
    sem_post(&(resources.projector_sem)); // sempahore wait
    simulator_complete = 1;
    return NULL;
}

pthread_t start_input_thread(TimerArgs* t_args)
{
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    char* resource_name = t_args->resource_name;
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

pthread_t start_output_thread(TimerArgs* t_args)
{
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    char* resource_name = t_args->resource_name;
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
    //printf("pid; %d\n",t_args->pcb->pid);

    return t_id;
}
