#include "Logger.h"
#include "Utils.h"
#include <unistd.h>
#include <stdio.h>
#include "Configuration.h"

char* LogTypeStrings[] = {"ERROR","Warning", "LOG"};
FILE* log_fp;
extern Configuration config;

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

void log_detailed_to(LogMode mode, LogType type, int line_number, char* error_in_file, char* log_text, char* log_file_path)
{
    char message_buffer[1024];
    char buffer[1025];
    sprintf(message_buffer,"%s:%d:%s: %s", error_in_file, line_number, LogTypeStrings[type], log_text); 
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
