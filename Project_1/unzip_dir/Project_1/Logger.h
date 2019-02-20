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
    LOGTYPE_WARNING,
    LOGTYPE_GENERAL

}LogType;

// used store a log message. can be printed later than the event occurs
typedef struct
{
    char consumed;//once a log is printed out, it's consumed (log message doesn't exist anymore)
    LogType type;
    int line_number; //on whiche line of the file the event occured
    char associated_file_name[128]; //file name where event occured
    char log_text[1024]; // message
}LogMessage;

void close_log_fp();

// various log functions
// with config uses logmode and log_file_path as defined in config in Simulator.c
void log_to(LogMode mode, char* log_text, char* log_file_path);
void log_detailed_to(LogMode mode, LogType type, int line_number, char* error_in_file, char* log_text, char* log_file_path);
void print_log_message(LogMode log_mode, char* log_file_path, LogMessage* log_message);
void print_log_message_with_config(LogMessage* log_message);
void set_error_log(LogMessage* log_message , int line_number, char* associated_file_name, char* log_text);
void log_with_config(char* log_text);
void log_detailed_with_config(LogType type, int line_number, char* error_in_file, char* log_text);
#endif
