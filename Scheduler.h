#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "MetaData.h"
#include "Utils.h"

void schedule_fifo(MetaData* metadata);
void schedule_sjf(MetaData* metadata);
void schedule_ps(MetaData* metadata);

#endif
