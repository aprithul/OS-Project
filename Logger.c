#include "Logger.h"
#include "Utils.h"
#include <unistd.h>
#include <stdio.h>

char* LogTypeStrings[] = {"ERROR","Warning", "LOG"};
FILE* log_fp;


void close_log_fp()
{
    if(log_fp!=NULL)
        fclose(log_fp);
}

void log_to(LogMode mode, char* log_text, char* log_file_name)
{
    char buffer[512];
    sprintf(buffer, "%s\n", log_text); 
    switch(mode)
    {
        case LOGMODE_MONITOR:
            printf("%s",buffer); 
            break;
        case LOGMODE_FILE:
            log_fp = write_to_file(buffer, log_file_name, log_fp); 
            break; 
        case LOGMODE_BOTH:
            printf("%s",buffer); 
            log_fp = write_to_file(buffer, log_file_name, log_fp); 
            break;
    }
}

void log_detailed_to(LogMode mode, LogType type, int line_number, char* error_in_file, char* log_text, char* log_file_name)
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
            log_fp = write_to_file(buffer, log_file_name, log_fp); 
            break; 
        case LOGMODE_BOTH:
            printf("%s",buffer); 
            log_fp = write_to_file(buffer, log_file_name, log_fp); 
            break;
    }
}

void print_log_message(LogMode log_mode, char* log_file_name, LogMessage* log_message)
{
    log_detailed_to(log_mode, log_message->type, log_message->line_number, log_message->associated_file_name, log_message->log_text, log_file_name);
}

