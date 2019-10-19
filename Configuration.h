#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Logger.h"
#include "CpuSchedulingCodes.h"

typedef struct
{
    char config_file_name[256];
    char version[128];
    char md_file_path[256];
    unsigned int monitor_display_time;
    unsigned int processor_cycle_time;
    unsigned int scanner_cycle_time;
    unsigned int hard_drive_cycle_time;
    unsigned int keyboard_cycle_time;
    unsigned int memory_cycle_time;
    unsigned int projector_cycle_time;
    unsigned int allocated_memory_size;
    unsigned int memory_block_size;
    unsigned int hard_drive_count;
    unsigned int projector_count;
    CpuSchedulingCodes csc;
    unsigned int quantum;
    LogMode log_target; 
    char log_file_path[256];
    LogMessage error_log;
    char constructed;
}Configuration;

void print_config(Configuration* config);
void print_config_cycles(Configuration* config);
int construct_configuration(Configuration* config, char* config_file_name);
int get_matching_key_index(char* key);
void recheck_config();
#endif
