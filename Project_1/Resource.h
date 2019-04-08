#ifndef RESOURCES_H
#define RESOURCES_H

#include <pthread.h>
#include <semaphore.h>


typedef struct
{
    int projector_max_count;
    int hdd_max_count;
    int projector_count;
    int hdd_count;

    sem_t projector_sem;
    sem_t hdd_sem;
    pthread_mutex_t keyboard_mutex;
    pthread_mutex_t scanner_mutex;
    pthread_mutex_t monitor_mutex;
    pthread_mutex_t projector_mutex;
}Resources;

void initialize_resource_manager(Resources* resources, int p_c, int hdd_c);

#endif
