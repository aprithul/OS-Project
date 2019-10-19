#include "Scheduler.h"

void schedule_fifo(MetaData* metadata)
{
    //do nothing since fifo is maintained by default
}

void schedule_sjf(MetaData* metadata)
{
   sort_bubble(metadata->process_task_counts, metadata->process_id,metadata->process_count, 1);
}

void schedule_ps(MetaData* metadata)
{
//    int _i = 0;
//    for(; _i<metadata->process_count; _i++)
//        printf("%d, ", metadata->process_priorities[_i]);
//    printf("\n");
   sort_bubble(metadata->process_priorities, metadata->process_id,metadata->process_count, 0);

//    _i = 0;
//    for(; _i<metadata->process_count; _i++)
//        printf("%d, ", metadata->process_priorities[_i]);
}
