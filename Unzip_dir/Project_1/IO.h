#ifndef IO_H
#define IO_H
#include <pthread.h>
#include <stdlib.h>
#include "Timer.h"
#include "PCB.h"

 
pthread_t start_input_thread(TimerArgs* t_args, PCB* pcb);
pthread_t start_output_thread(TimerArgs* t_args, PCB* pcb);
void* input_thread(void* i_args);
void* output_thread(void* i_args);
#endif
