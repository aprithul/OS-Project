#ifndef IO_H
#define IO_H
#include <pthread.h>
#include <stdlib.h>
#include "Timer.h"
#include "Resource.h"

extern Resources resources;

pthread_t start_input_thread(TimerArgs* t_args);
pthread_t start_output_thread(TimerArgs* t_args);

void* hdd_input_thread(void* i_args);
void* keyboard_input_thread(void* i_args);
void* scanner_input_thread(void* i_args);
void* hdd_output_thread(void* i_args);
void* monitor_output_thread(void* i_args);
void* projector_output_thread(void* i_args);
#endif
