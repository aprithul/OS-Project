#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "LinkedList.h"
#include "Utils.h"
#include "Logger.h"
#include "MetaData.h"
#include "Configuration.h"

//j#define to_string(node) ((char*)(node->value))
//LinkedListmdb* construct_metadata(char* meta_data_text);
Configuration config;
int main(int argc, char *argv[])
{
    // will be set to 1 once configuratin is constructed
    // required for logging using *_with_config functions
    config.constructed = 0;
    //construct metadata
    if(argc>1)
    {
        char message_buffer[512];
        char* config_file_name = argv[1];
        int len = 0;
        char del[] = {'.','\0'}; 
        char** split_text = split(config_file_name, del, &len);
        // check if file name has proper format
        // has two parts with the second part being 'conf', there are no spaces in the name and has only one '.' character
        if(len!=2 || strchr(split_text[0], ' ') != NULL || get_char_count('.', config_file_name) != 1 || strcmp(split_text[1], "conf"))
        {
            log_detailed_to(LOGMODE_MONITOR, LOGTYPE_ERROR,0, NULL, "Invalid configuration file name", NULL);
            return -1; 
        }
        // file doesn't exist 
        if(access(config_file_name, F_OK) == -1)
        {
            sprintf(message_buffer, "File not found : %s", config_file_name); 
            log_detailed_to(LOGMODE_MONITOR, LOGTYPE_ERROR,0, NULL, message_buffer, NULL);
            //set_error_log(&metadata->error_log,0,metadata_file_path, message_buffer);        
            return -1;
        }

        free_split_text(split_text,len); 
       
         
        int error_code = construct_configuration(&config,config_file_name);
        if(error_code>=0)//-1 means fatal error
            print_config(&config); // logs config file
        
        if(error_code == 0) // if there was no error in configuration file, read metadata file
        {
            config.constructed = 1; 
            MetaData metadata;
            error_code = construct_metadata(&metadata,config.md_file_path);
            //printf("%d:error code \n", had_error);  
            if(error_code>=0) // if metadata file returns <0 , it means either md file doesn't exist or file is empty. otherwise print the values
                print_metadata(&config, &metadata);
        }
                
        //construct_metadata("helo : world");   

    }
    else
    {
        log_detailed_to(LOGMODE_MONITOR, LOGTYPE_ERROR,0, NULL, "No configuration file specified", NULL);
        return -1; 
    }
    
    close_log_fp();//closes the log file before program exists
    return 0; 
}
