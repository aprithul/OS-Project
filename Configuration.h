#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Logger.h"

typedef struct
{
    char version[128];
    char md_file_path[256];
    unsigned int monitor_display_time;
    unsigned int processor_cycle_time;
    unsigned int scanner_cycle_time;
    unsigned int hard_drive_cycle_time;
    unsigned int keyboard_cycle_time;
    unsigned int memory_cycle_time;
    unsigned int projector_cycle_time;
    LogMode log_target; 
    char log_file_path[256];
   
    char error_flag; 
    LogMessage error_log;
}Configuration;

void print_config(Configuration* config);
void print_config_cycles(Configuration* config);
int construct_configuration(Configuration* config, char* config_file_name);
int get_matching_key_index(char* key);
void set_error_log(Configuration* config, int line_number, char* error_in_file, char* log_text);
#endif
