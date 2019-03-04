#include <string.h>
typedef enum
{
    START = 0,
    READY = 1,
    RUNNING = 2,
    WAITING = 3,
    EXIT = 4
}ProcessState;


typedef struct
{
    ProcessState process_state;    
    int pid;
}PCB;
