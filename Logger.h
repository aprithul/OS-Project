#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <string.h>

typedef enum 
{
    LOGMODE_MONITOR,
    LOGMODE_FILE,
    LOGMODE_BOTH
}LogMode;

typedef enum
{
    LOGTYPE_ERROR,
    LOGTYPE_GENERAL

}LogType;

typedef struct
{
    char consumed;
    LogType type;
    int line_number;
    char associated_file_name[128];
    char log_text[1024];
}LogMessage;

void close_log_fp();
void log_to(LogMode mode, char* log_text, char* log_file_path);
void log_detailed_to(LogMode mode, LogType type, int line_number, char* error_in_file, char* log_text, char* log_file_path);
void print_log_message(LogMode log_mode, char* log_file_path, LogMessage* log_message);
void print_log_message_with_config(LogMessage* log_message);
void set_error_log(LogMessage* log_message , int line_number, char* associated_file_name, char* log_text);
void log_with_config(char* log_text);
void log_detailed_with_config(LogType type, int line_number, char* error_in_file, char* log_text);
#endif
