#include "Resource.h"

void initialize_resource_manager(Resources* resources, int p_c, int hdd_c)
{
    resources->projector_max_count = p_c;
    resources->hdd_max_count = hdd_c;
    resources->projector_count = 0;
    resources->hdd_count = 0;

    sem_init(&(resources->projector_sem), 0, p_c);
    sem_init(&(resources->hdd_sem), 0, hdd_c);
    pthread_mutex_init(&(resources->keyboard_mutex), NULL);
    pthread_mutex_init(&(resources->scanner_mutex), NULL);
    pthread_mutex_init(&(resources->monitor_mutex), NULL);
    pthread_mutex_init(&(resources->projector_mutex), NULL);
}


