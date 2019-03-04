#include <unistd.h>
#include <stdio.h>
#include "Logger.h"
#include "Utils.h"
#include "Configuration.h"


char* LogTypeStrings[] = {"ERROR","WARNING", "LOG"};
FILE* log_fp; // file pointer to log file
extern Configuration config; // available when configuration file has been constructed. Logging can be done with the *_with_config variants onwards                             // without specifying log mode or log file path (if logging to file)

//closes file pointer to log file
//call when logging is done
void close_log_fp()
{
    if(log_fp!=NULL)
        fclose(log_fp);
}


void log_to(LogMode mode, char* log_text, char* log_file_path)
{
    char buffer[512];
    sprintf(buffer, "%s\n", log_text); 
    switch(mode)
    {
        case LOGMODE_MONITOR:
            printf("%s",buffer); 
            break;
        case LOGMODE_FILE:
            log_fp = write_to_file(buffer, log_file_path, log_fp); 
            break; 
        case LOGMODE_BOTH:
            printf("%s",buffer); 
            log_fp = write_to_file(buffer, log_file_path, log_fp); 
            break;
    }
}

// depending on the parameters provided the output can look different
void log_detailed_to(LogMode mode, LogType type, int line_number, char* error_in_file, char* log_text, char* log_file_path)
{
    // buffer is one greater since it adds a new line character
    char message_buffer[1024];
    char buffer[1025];
    if(error_in_file==NULL)
        sprintf(message_buffer,"\033[1;31m%s\033[0m: %s", LogTypeStrings[type], log_text); 
    else if(line_number == 0)
        sprintf(message_buffer,"%s:\033[1;31m%s\033[0m: %s", error_in_file, LogTypeStrings[type], log_text); 
    else
        sprintf(message_buffer,"%s:\033[1;35m%d\033[0m::\033[1;31m%s\033[0m: %s", error_in_file, line_number, LogTypeStrings[type], log_text); 
    sprintf(buffer, "%s\n", message_buffer); 
    switch(mode)
    {
        case LOGMODE_MONITOR:
            printf("%s",buffer); 
            break;
        case LOGMODE_FILE:
            log_fp = write_to_file(buffer, log_file_path, log_fp); 
            break; 
        case LOGMODE_BOTH:
            printf("%s",buffer); 
            log_fp = write_to_file(buffer, log_file_path, log_fp); 
            break;
    }
}

void log_with_config(char* log_text)
{   
    if(config.constructed)
        log_to(config.log_target, log_text, config.log_file_path);
    else
        log_to(LOGMODE_MONITOR, "Configuration not found", NULL);
}

void log_detailed_with_config(LogType type, int line_number, char* error_in_file, char* log_text)
{
    if(config.constructed) 
        log_detailed_to(config.log_target, type, line_number,  error_in_file, log_text, config.log_file_path);
    else
        log_to(LOGMODE_MONITOR, "Configuration not found", NULL);
}
void print_log_message_with_config(LogMessage* log_message)
{
    if(config.constructed)
        log_detailed_with_config(log_message->type, log_message->line_number, log_message->associated_file_name, log_message->log_text);
    else
        log_to(LOGMODE_MONITOR, "Configuration not found", NULL);
}

void print_log_message(LogMode log_mode, char* log_file_path, LogMessage* log_message)
{
    log_detailed_to(log_mode, log_message->type, log_message->line_number, log_message->associated_file_name, log_message->log_text, log_file_path);

    log_message->consumed = 1;
}

//creates a log message at the given pointer, useful if you need to store a log message to print later
void set_error_log(LogMessage* log_message , int line_number, char* associated_file_name, char* log_text)
{
    if(log_message->consumed)
    {
        log_message->consumed = 0;
        log_message->type = LOGTYPE_ERROR;
        log_message->line_number = line_number;
        strcpy(log_message->associated_file_name, associated_file_name);
        strcpy(log_message->log_text, log_text);
    }
}
