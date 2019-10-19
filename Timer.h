#ifndef TIMER_H
#define TIMER_H
#include <sys/time.h>
#include <pthread.h>
#include "PCB.h"

#define to_mic_sec(x) (x*1e6)
typedef struct
{
    void (*callback)();
    unsigned long duration;
    char* resource_name;
    PCB* pcb;
}TimerArgs;

unsigned long get_current_time();
void set_timer(TimerArgs* args);
void set_timer_async(TimerArgs* args);
void* timer_countdown(void* args);
double mic_to_sec(unsigned long micro_sec);
double mic_to_milsec(unsigned long micro_sec);

#endif
