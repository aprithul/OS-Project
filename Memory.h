#ifndef MEMORY_H
#define MEMORY_H
#include "Timer.h"
void allocate_memory(TimerArgs* t_args, unsigned int* address, int size, int async);
void block_memory(TimerArgs* t_args);
#endif
