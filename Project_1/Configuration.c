#include "Configuration.h"
#include "Logger.h"
#include "Utils.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// considers configuration file as lines of key value pair separated by ':' ( except first and last line)
// following are the keys
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

// component flag has following order
//  {empty} , version, filep path, monitor, processor, scanner, hard drive, keyboard, memory, projector, log, log file path
int component_flag[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


// construct_configuration Error codes:
//      1 = Error, will create partial Metadata
//      0 = No Error
//     -1 = Fatal Error, returning immediately

// parse the 'config_text' and store the data in the 'config' struct
// return -1 if construction fails, return 1 on success
int construct_configuration(Configuration* config, char* config_file_name)
{
    int error_code = 0;
    // start with no error log records aka all error logs are consumed 
    config->error_log.consumed = 1; 
    char* config_text = read_file(config_file_name);
    if(!is_text_whitespace(config_text))
    {
        char del[] = {'\n','\0'}; 
        //split config text into lines 
        int config_len = 0;
        char** split_by_new_line = split(config_text, del, &config_len);   

        int _i = 1,_end=config_len-1;
        if( strcmp(strip(split_by_new_line[0]),configuration_keys[0]))
        {
            log_detailed_to(LOGMODE_MONITOR,LOGTYPE_ERROR, 0,config_file_name,"No Start Simulator Statement in config file, exiting", NULL);
            error_code = -1;
            _i = 0;
        }
        if( strcmp(strip(split_by_new_line[_end]),configuration_keys[12]))
        {
            log_detailed_to(LOGMODE_MONITOR,LOGTYPE_ERROR, 0,config_file_name,"No End Simulator Statement in config file, exiting", NULL);
            error_code = -1;
            _end++;
        }      
        
        // parse each line 
        for( ; _i<_end; _i++)
        {
            if(is_text_whitespace(split_by_new_line[_i]))
                continue;

            char col_del[]= {':','\0'};
            // split lines into key-value  pairs by ':'
            int colon_split_len = 0;
            char** split_by_colon = split(split_by_new_line[_i],col_del , &colon_split_len);
            if(colon_split_len != 2)
            { 
                //set_error_log( &(config->error_log), (_i+1), config_file_name, "Must be a single ':' separated"); 
//              printf("%s %s %s ", split_by_colon[0], split_by_colon[1], split_by_colon[2]);
                //free_split_text(split_by_colon, colon_split_len);
                error_code = 1; 
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
            switch(key_index)
            {
                case 1:// version
                    strcpy(config->version,value);                     
                    component_flag[key_index] = 1; 
                    break;
                case 2:// metadata file name
                    file_name_len = 0;
                    split_file_name = split(value,del_2,&file_name_len); 
                    if(file_name_len == 2 && strchr(split_file_name[0],' ')==NULL && get_char_count('.',value)==1 && !strcmp(split_file_name[1], "mdf"))
                        strcpy(config->md_file_path, value);
                    else
                    {
                        strcpy(config->md_file_path, "\0");
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid metadata file name"); 
                        error_code = 1; 
                    }
                    free_split_text(split_file_name, file_name_len); 
                    component_flag[key_index] = 1; 
                    break;
                case 3://Monitor
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                       val = 0; 
                       set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid monitor display time"); 
                       error_code = 1; 
                    }
                    config->monitor_display_time = (int)val; 
                    component_flag[key_index] = 1; 
                    break;
                case 4://Processor 
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                        val = 0; 
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid processor cycle time"); 
                        error_code = 1; 
                    }
                    config->processor_cycle_time =(int) val; 
                    component_flag[key_index] = 1; 
                    break;
                case 5://Scanner 
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                        val = 0;
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid scanner cycle time"); 
                        error_code = 1; 
                    }
                    config->scanner_cycle_time = (int)val; 
                    component_flag[key_index] = 1; 
                    break;
                case 6://Hard Drive 
                    
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                        val = 0;
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid hard drive cycle time"); 
                        error_code = 1; 
                    }
                    config->hard_drive_cycle_time = (int)val; 
                    component_flag[key_index] = 1; 
                    break;
                case 7://Keyboard 
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                        val = 0;
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid keyboard cycle time"); 
                        error_code = 1; 
                    }
                    config->keyboard_cycle_time = (int)val; 
                    component_flag[key_index] = 1; 
                    break;
                case 8://Memory
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                        val = 0;
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid memory cycle time"); 
                        error_code = 1; 
                    }
                    config->memory_cycle_time = (int)val;
                    component_flag[key_index] = 1; 
                    break;
                case 9://Projector 
                    val = strtol(value, &temp, 10);
                    if(val <=0 || val >= INT_MAX || *temp != '\0')
                    {
                        val = 0;
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid projector cycle time"); 
                        error_code = 1; 
                    }
                    config->projector_cycle_time = (int)val; 
                    component_flag[key_index] = 1; 
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
                        error_code = 1; 
                    }    
                    component_flag[key_index] = 1; 
                break;
                case 11://Log file 
                    file_name_len = 0; 
                    split_file_name = split(value, del_2,&file_name_len); 
                    if(file_name_len == 2 && strchr(split_file_name[0],' ')==NULL && get_char_count('.',value)==1 && !strcmp(split_file_name[1], "lgf"))
                        strcpy(config->log_file_path, value);
                    else
                    {
                        config->log_target = LOGMODE_MONITOR;
                        strcpy(config->log_file_path, "\0");
                        set_error_log(&(config->error_log), (_i+1), config_file_name, "Invalid log file name provided, will log to monitor"); 
                        error_code = 1; 
                    }
                    free_split_text(split_file_name, file_name_len); 
                    component_flag[key_index] = 1; 
                break;
                default://key couldn't be found in list of keys
 //                   printf("%s\n", value);
                    set_error_log(&(config->error_log), (_i+1), config_file_name, "Parsing error, check configuration file"); 
                    error_code = 1; 
            }
           //printf("%s\n",split_by_colon[1]);      
            free_split_text(split_by_colon,colon_split_len );
        }
        free_split_text(split_by_new_line, config_len);
        
        //critical errors 
        if(component_flag[10] == 0) // log statement missing
        {
            config->log_target = LOGMODE_MONITOR; 
            log_detailed_to(config->log_target,LOGTYPE_ERROR, 0, config_file_name, "Log target not found", NULL);
            error_code = -1;
        }
        else if(component_flag[11] == 0 && config->log_target != LOGMODE_MONITOR)//log file path
        {
            config->log_target = LOGMODE_MONITOR; 
            log_detailed_to(config->log_target,LOGTYPE_ERROR, 0, config_file_name, "Requested logging to file, but not file path provided",NULL);
            error_code = -1;
        }
        else if(component_flag[1] == 0)//version/phase missing
        {
            log_detailed_to(config->log_target,LOGTYPE_ERROR, 0, config_file_name, "Version/Phase number not found", config->log_file_path);
            error_code = -1; 
        }
        else if(component_flag[2] == 0)//metadata file path missing
        {
            log_detailed_to(config->log_target,LOGTYPE_ERROR, 0, config_file_name, "Meta-Data file path not found", config->log_file_path);
            error_code = -1;
        }
    }
    else
    {
        log_detailed_to(LOGMODE_MONITOR,LOGTYPE_ERROR, 0,config_file_name, "Configuration file is empty, exiting", NULL);
//        set_error_log(&(config->error_log), 0, config_file_name, "ERROR: Configuration file is empty"); 
        error_code = -1; 
    }   
    return error_code;
}



// logs the cycle values for various componenets
// logs saved error if value is invalid
// only log until an error is encountered 
void print_config_cycles(Configuration* config)
{
    
    char buffer[512]; 
    //different from componenet flag[2] being 0, which means no metadata file path statement was present in config file
    //however if length is 0, it means there was some issue while parsing file name
    if(strlen(config->md_file_path) == 0)
    {
        print_log_message(config->log_target, config->log_file_path, &config->error_log);
        return;
    }
    
    // config file may not contain cycle times for every componenet. However metadata file shouldn't try to access those values. so assign 0 to display/cycle time if componenet flag is 0
    if(component_flag[3]) 
    {
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
    }
    else config->monitor_display_time = 0; 
     
    if(component_flag[4]) 
    {
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
    } 
    else config->processor_cycle_time = 0; 
    
    if(component_flag[5])
    {
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
    } 
    else config->scanner_cycle_time = 0; 
    
    if(component_flag[6])
    {
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
    } 
    else config->hard_drive_cycle_time = 0; 
    
    if(component_flag[7])
    {
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
    }
    else config->keyboard_cycle_time = 0; 
    
    if(component_flag[8]) 
    {
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
    }
    else config->memory_cycle_time = 0; 
    
    if(component_flag[9]) 
    {
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
    else config->projector_cycle_time = 0; 
}

//log the config file values
void print_config(Configuration* config)
{
    // print config file values until error is faced
    /////////////////////////////////////////////// 

    char buffer[512]; 
    log_to(config->log_target,"Configuration File Data", config->log_file_path);
        
    // print cycle times for different components
    print_config_cycles(config); 

    // report any outstanding error message
    if(!config->error_log.consumed)
        print_log_message(config->log_target, config->log_file_path, &config->error_log);
   
    //print log target ( logged to x and/or y )?
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


