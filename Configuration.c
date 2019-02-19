#include "Configuration.h"
#include "Logger.h"
#include "Parser.h"
#include "Utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
char* configuration_keys[] = {
                            "Start Simulator Configuration File",
                            "Version/Phase",
                            "File Path",
                            "Monitor display time {msec}",
                            "Processor cycle time {msec}",
                            "Scanner cycle time {msec}",
                            "Hard drive cycle time {msec}",
                            "Keyboard cycle time {msec}",
                            "Memory cycle time {msec}",
                            "Projector cycle time {msec}",
                            "Log",
                            "Log File Path",
                            "End Simulator Configuration File"
                           };

// parse the 'config_text' and store the data in the 'config' struct
// return -1 if construction fails, return 1 on success
int construct_configuration(Configuration* config, char* config_file_name)
{
    int error_found = 0;
    // start with no error log records aka all error logs are consumed 
    config->error_log.consumed = 1; 
    char* config_text = read_file(config_file_name);
    if(!is_text_whitespace(config_text))
    {
        char del[] = {'\n','\0'}; 
        //split config text into lines 
        int config_len = 0;
        char** split_by_new_line = split(config_text, del, &config_len);   

        int _i = 1;
        for( ; _i<config_len-1; _i++)
        {
            char col_del[]= {':','\0'};
            // split lines into key-value  pairs by ':'
            int colon_split_len = 0;
            char** split_by_colon = split(split_by_new_line[_i],col_del , &colon_split_len);
            if(colon_split_len != 2)
            {
                set_error_log( &(config->error_log), (_i+1), config_file_name, "Must be ':' separated"); 
                free_split_text(split_by_colon, colon_split_len);
                error_found = 1; 
            }

            //consider the part before ':' as the key and the part after ':' as the value
            char* key = strip(make_singly_spaced(split_by_colon[0]));
            char* value = strip(split_by_colon[1]);
            
            // get the index of the key from keys list
            int key_index = get_matching_key_index(key);
            // variables to be used inside the struct
            long int val = 0;
            char* temp = NULL; 
            char* log_mode = NULL;
            char** split_file_name = NULL;
            int file_name_len = 0;
            char del_2[] = {'.','\0'};
            //parse value appropriately depending on the key 
            //memory management done immediately on fail 
            switch(key_index)
            {
                case 1:// version
                    strcpy(config->version,value);                      
                    break;
                case 2:// metadata file name
                    file_name_len = 0;
                    split_file_name = split(value,del_2,&file_name_len); 
                    if(file_name_len == 2 && strchr(split_file_name[0],' ')==NULL && !strcmp(split_file_name[1], "mdf"))
                        strcpy(config->md_file_path, value);
                    else
                    {
                        strcpy(config->md_file_path, "\0");
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid metadata file name"); 
                        error_found = 1; 
                    }
                    free_split_text(split_file_name, file_name_len); 
                    break;
                case 3://Monitor
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                       val = 0; 
                       set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid monitor display time"); 
                       error_found = 1; 
                    }
                    config->monitor_display_time = (int)val; 
                    break;
                case 4://Processor 
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                        val = 0; 
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid processor cycle time"); 
                        error_found = 1; 
                    }
                    config->processor_cycle_time =(int) val; 
                    break;
                case 5://Scanner 
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                        val = 0;
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid scanner cycle time"); 
                        error_found = 1; 
                    }
                    config->scanner_cycle_time = (int)val; 
                    break;
                case 6://Hard Drive 
                    
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                        val = 0;
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid hard drive cycle time"); 
                        error_found = 1; 
                    }
                    config->hard_drive_cycle_time = (int)val; 
                    break;
                case 7://Keyboard 
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                        val = 0;
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid keyboard cycle time"); 
                        error_found = 1; 
                    }
                    config->keyboard_cycle_time = (int)val; 
                    break;
                case 8://Memory
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                        val = 0;
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid memory cycle time"); 
                        error_found = 1; 
                    }
                    config->memory_cycle_time = (int)val;
                    break;
                case 9://Projector 
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                        val = 0;
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid projector cycle time"); 
                        error_found = 1; 
                    }
                    config->projector_cycle_time = (int)val; 
                    break;
                case 10://Log mode
                    log_mode = strip(make_singly_spaced(value));
                    if(!strcmp(value, "Log to Both"))
                        config->log_target = LOGMODE_BOTH;
                    else if(!strcmp(value, "Log to Monitor"))
                        config->log_target = LOGMODE_MONITOR;
                    else if(!strcmp(value, "Log to File"))
                        config->log_target = LOGMODE_FILE;
                    else
                    {
                        config->log_target = LOGMODE_MONITOR;
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid log target provided, will log to Monitor"); 
                        error_found = 1; 
                    }    
                break;
                case 11://Log file 
                    file_name_len = 0; 
                    split_file_name = split(value, del_2,&file_name_len); 
                    if(file_name_len == 2 && strchr(split_file_name[0],' ')==NULL && !strcmp(split_file_name[1], "lgf"))
                        strcpy(config->log_file_path, value);
                    else
                    {
                        config->log_target = LOGMODE_MONITOR;
                        strcpy(config->log_file_path, "\0");
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid log file name provided, will log to monitor"); 
                        error_found = 1; 
                    }
                    free_split_text(split_file_name, file_name_len); 
                break;
                default://key couldn't be found in list of keys
                    set_error_log(&(config->error_log), (_i+1), config_file_name, "Not a valid key"); 
                    error_found = 1; 
            }
           //printf("%s\n",split_by_colon[1]);      
          // free_split_text(split_by_colon,colon_split_len );
        }
        free_split_text(split_by_new_line, config_len);
    }
    else
    {
        set_error_log(&(config->error_log), 0, config_file_name, "Configuration file is empty"); 
        error_found = 1; 
    }   
    
    return error_found;
}


void print_config_cycles(Configuration* config)
{
    char buffer[512]; 
    if(strlen(config->md_file_path) == 0)
    {
        print_log_message(config->log_target, config->log_file_path, &config->error_log);
        return;
    }
    if(config->monitor_display_time > 0) 
    {
        sprintf(buffer, "Monitor = %d ms/cycle", config->monitor_display_time); 
        log_to(config->log_target, buffer, config->log_file_path); 
     }
    else
    {
        print_log_message(config->log_target, config->log_file_path, &config->error_log);
        return;
    } 
    
    if(config->processor_cycle_time > 0) 
    {
        sprintf(buffer, "Processor = %d ms/cycle", config->processor_cycle_time); 
        log_to(config->log_target, buffer, config->log_file_path); 
    }
    else
    {
        print_log_message(config->log_target, config->log_file_path, &config->error_log);
        return;
    } 
    

    if(config->scanner_cycle_time > 0) 
    {
        sprintf(buffer, "Scanner = %d ms/cycle", config->scanner_cycle_time); 
        log_to(config->log_target, buffer, config->log_file_path); 
    }
    else
    {
        print_log_message(config->log_target, config->log_file_path, &config->error_log);
        return;
    } 
    
    if(config->hard_drive_cycle_time > 0) 
    {
        sprintf(buffer, "Hard Drive = %d ms/cycle", config->hard_drive_cycle_time); 
        log_to(config->log_target, buffer, config->log_file_path); 
    }
    else
    {
        print_log_message(config->log_target, config->log_file_path, &config->error_log);
        return;
    } 
    
    if(config->keyboard_cycle_time > 0) 
    {
        sprintf(buffer, "Keyboard = %d ms/cycle", config->keyboard_cycle_time); 
        log_to(config->log_target, buffer, config->log_file_path); 
    }
    else
    {
        print_log_message(config->log_target, config->log_file_path, &config->error_log);
        return;
    } 
    
    if(config->memory_cycle_time > 0) 
    {
        sprintf(buffer,"Memory = %d ms/cycle", config->memory_cycle_time); 
        log_to(config->log_target, buffer, config->log_file_path); 
    }
    else
    {
        print_log_message(config->log_target, config->log_file_path, &config->error_log);
        return;
    } 
    
    if(config->projector_cycle_time > 0) 
    {
        sprintf(buffer, "Projector = %d ms/cycle", config->projector_cycle_time); 
        log_to(config->log_target, buffer, config->log_file_path); 
    }
    else
    {
        print_log_message(config->log_target, config->log_file_path, &config->error_log);
        return;
    } 

}

void print_config(Configuration* config)
{
    // print config file values until error is faced
    char buffer[512]; 
    log_to(config->log_target,"Configuration File Data", config->log_file_path);
    print_config_cycles(config); 
    
    // logged to? 
    if(config->log_target == LOGMODE_MONITOR)
    {
        log_to(config->log_target, "Logged to : monitor", config->log_file_path); 
    }
    else if(config->log_target == LOGMODE_FILE)
    {
        sprintf(buffer,"Logged to : %s\n", config->log_file_path);
        log_to(config->log_target, buffer, config->log_file_path); 
    }    
    else if(config->log_target == LOGMODE_BOTH)
    {
        sprintf(buffer,"Logged to : monitor and %s\n", config->log_file_path);
        log_to(config->log_target, buffer, config->log_file_path); 
    }
    // report any outstanding error message
    if(!config->error_log.consumed)
        print_log_message(config->log_target, config->log_file_path, &config->error_log);
} 

// searches for the ky in list of keys as defined above
// returns -1 if key not found
int get_matching_key_index(char* tag)
{
    int _i = 1;
    for( ; _i<12; _i++)
    {
        if(!strcmp(tag,configuration_keys[_i]))
            return _i; 
    }
    return -1;
}


