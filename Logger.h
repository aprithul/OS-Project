#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
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
    LogType type;
    int line_number;
    char associated_file_name[128];
    char log_text[1024];
}LogMessage;

void close_log_fp();
void log_to(LogMode mode, char* log_text, char* log_file_name);
void log_detailed_to(LogMode mode, LogType type, int line_number, char* error_in_file, char* log_text, char* log_file_name);
void print_log_message(LogMode log_mode, char* log_file_name, LogMessage* log_message);
#endif
