#include "Logger.h"
char* LogTypeStrings[] = {"ERROR : ", ""};
void log_to(LogMode mode, LogType type, char* log_text)
{

    switch(mode)
    {
        case LOGMODE_MONITOR:
            printf("%s%s\n",LogTypeStrings[type],log_text); 
            break;
        case LOGMODE_FILE:
            break; 
        case LOGMODE_BOTH:
            break;

    }


}
