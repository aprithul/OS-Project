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


void log_to(LogMode mode, LogType type, char* log_text);


#endif
