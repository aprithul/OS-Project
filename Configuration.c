#include "Configuration.h"
#include "Logger.h"
#include "Parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* configuration_header = "Start Simulator Configuration File";
char* configuration_footer = "End Simulator Configuration File";
char* configuration_tags[] = {
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
                            "Log File Path"
                           };
int construct_configuration(Configuration* config, char* config_text)
{
    int config_len = 0;
    char** split_by_new_line = split(config_text, '\n', &config_len);    
    if(config_len>0)
    {
        char* header = split_by_new_line[0];
         
        if(strcmp(header, configuration_header))
            log_to(LOGMODE_MONITOR, LOGTYPE_ERROR, "Configuration file doesn't have start block");  
        
        int _i = 1;
        for( ; _i<config_len-1; _i++)
        {
            // split file lines into 2 parts by ':'
            int colon_split_len = 0;
            char**split_by_colon = split(split_by_new_line[_i], ':', &colon_split_len);
            if(colon_split_len != 2)
            {
                log_to(LOGMODE_MONITOR, LOGTYPE_ERROR, "Error in configuration file"); 
                printf("%d \n", colon_split_len);
                
                //cleanup before return
                free_split_text(split_by_new_line, config_len);
                free_split_text(split_by_colon, colon_split_len);
                return -1;
            }
            
            char* tag = strip(make_singly_spaced(split_by_colon[0]));
            //log_to(LOGMODE_MONITOR, LOGTYPE_GENERAL, l); 
            int tag_index = get_matching_tag_index(tag);
            switch(tag_index)
            {
                case 0:
                    config->version = split_by_colon[1];                      
                break;
                case 1: 
                    config->md_file_path = split_by_colon[1];
                break;
                case 2:
                    config->monitor_display_time = atoi(split_by_colon[1]); 
                break;
                case 3: 
                    config->processor_cycle_time = atoi(split_by_colon[1]); 
                break;
                case 4: 
                    config->scanner_cycle_time = atoi(split_by_colon[1]); 
                break;
                case 5: 
                    config->hard_drive_cycle_time = atoi(split_by_colon[1]); 
                break;
                case 6: 
                    config->keyboard_cycle_time = atoi(split_by_colon[1]); 
                break;

            } 
         }
       print_config(config);
    }
    else
    {
        log_to(LOGMODE_MONITOR, LOGTYPE_ERROR, "Configuration file is empty");      
    }   
    return 1; 
}

void print_config(Configuration* config)
{
    char buffer[20];
    sprintf(buffer,"%d", config->processor_cycle_time);
    log_to(LOGMODE_MONITOR, LOGTYPE_GENERAL,buffer  );      
}

int get_matching_tag_index(char* tag)
{
    int _i = 0;
    for( ; _i<11; _i++)
    {
        if(!strcmp(tag,configuration_tags[_i]))
            return _i; 
    }
    return -1;
}
