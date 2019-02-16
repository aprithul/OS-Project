#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Logger.h"

typedef struct
{
    char* version;
    char* md_file_path;
    unsigned int monitor_display_time;
    unsigned int processor_cycle_time;
    unsigned int scanner_cycle_time;
    unsigned int hard_drive_cycle_time;
    unsigned int keyboard_cycle_time;
    unsigned int memory_cycle_time;
    unsigned int projector_cycle_time;
    LogMode log_target; 
    char* log_file_path;
}Configuration;

void print_config(Configuration* config);
int construct_configuration(Configuration* config, char* config_text);
int get_matching_tag_index(char* tag);
#endif
