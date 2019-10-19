#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "LinkedList.h"
#include "Utils.h"
#include "Logger.h"
#include "MetaData.h"
#include "Configuration.h"
#include "Memory.h"
#include "IO.h"

//j#define to_string(node) ((char*)(node->value))
//LinkedListmdb* construct_metadata(char* meta_data_text);
extern char codes[];
extern char* descriptors[];
extern int get_total_cycle_time(Configuration* config, MetaDataRecord* mdr);

unsigned long s_t,e_t;
Configuration config;
void execute_program(MetaData* metadata);
double simulator_start_time;


int main(int argc, char *argv[])
{
    //TimerArgs t_args;
    //t_args.callback = callback;
    //t_args.duration = to_sec(5);

    // return 0;
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
        //if(error_code>=0)//-1 means fatal error
        //    print_config(&config); // logs config file
        
        if(error_code == 0) // if there was no error in configuration file, read metadata file
        {
            config.constructed = 1; 
            MetaData metadata;
            error_code = construct_metadata(&metadata,config.md_file_path);
            //printf("%d:error code \n", had_error);  
            if(error_code>=0) // if metadata file returns <0 , it means either md file doesn't exist or file is empty. otherwise print the values
            {
                //print_metadata(&config, &metadata);
                execute_program(&metadata);
            } 
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


void execute_program(MetaData* md)
{
    char message_buffer[512];
    PCB* pcb; 
    int process_id = 0;
 
    if(md->records.count>0)
    {
        int i=0;
        for(;i<md->records.count;i++)
        {
            MetaDataRecord* mdr = (MetaDataRecord*)(ll_get(&(md->records),i)->value);
            int cycle_time = get_total_cycle_time(&config, mdr);
            TimerArgs t_args;
            t_args.duration = cycle_time*1000;//cycle time is in ms. so convert it to micro s.
            t_args.callback = NULL;
            //printf("%c\n", codes[mdr->code_id]);
            if(codes[mdr->code_id] == 'S')// && codes[mdr->code_id] != 'S')
            {
                if(!strcmp(descriptors[mdr->descriptor_id], "begin"))
                {
                    simulator_start_time = get_current_time();
                    sprintf(message_buffer, "%f - %s", mic_to_sec(get_current_time() - simulator_start_time),"Simulator program starting");
                    srand(time(0));// set the seed for memory allocation 
                } 
                if(!strcmp(descriptors[mdr->descriptor_id], "finish"))
                {
                    sprintf(message_buffer, "%f - %s", mic_to_sec(get_current_time() - simulator_start_time),"Simulator program ending");
                } 
                log_with_config(message_buffer);
            }
            else if(codes[mdr->code_id] == 'A')// && codes[mdr->code_id] != 'S')
            {
                if(!strcmp(descriptors[mdr->descriptor_id], "begin"))
                {
                    process_id++;
                    sprintf(message_buffer, "%f - %s %d", mic_to_sec(get_current_time() - simulator_start_time),"OS: preparing process",process_id);
                    log_with_config(message_buffer);
                    // create the process
                    // since the only thing we now have is the process state, assign appropriate value 
                    pcb = malloc(sizeof(PCB));  // create pcb for current process 
                    pcb->process_state = START;   // assign state
                    pcb->pid = process_id;

                    // done creating process
                    pcb->process_state = READY; // assign state
                    sprintf(message_buffer, "%f - %s %d", mic_to_sec(get_current_time() - simulator_start_time),"OS: starting process",pcb->pid);
                    log_with_config(message_buffer);
                } 
                if(!strcmp(descriptors[mdr->descriptor_id], "finish"))
                {
                    sprintf(message_buffer, "%f - %s %d", mic_to_sec(get_current_time() - simulator_start_time),"OS: removing process", pcb->pid);
                    log_with_config(message_buffer);
                    pcb->process_state = EXIT; // assign state
                    free(pcb);
                } 
 
            }
            else
            {   
                  
                pcb->process_state = RUNNING; // assign state
                if(codes[mdr->code_id] == 'P')
                {
                    if(!strcmp(descriptors[mdr->descriptor_id], "run"))
                    {
                        sprintf(message_buffer, "%f - Process %d: start processing action", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                        log_with_config(message_buffer);
                        pcb->process_state = WAITING;
                        set_timer(&t_args);   
                        pcb->process_state = RUNNING;
                        sprintf(message_buffer, "%f - Process %d: end processing action", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                        log_with_config(message_buffer);

                    }
                }
                else if(codes[mdr->code_id] == 'I')
                {
                        s_t = get_current_time() - simulator_start_time; 
                        sprintf(message_buffer, "%f - Process %d: start %s input", mic_to_sec(s_t), pcb->pid, descriptors[mdr->descriptor_id]);
                        log_with_config(message_buffer);

                        pcb->process_state = WAITING;
                        pthread_join(start_input_thread(&t_args, pcb),NULL);// start an input thread
                        pcb->process_state = RUNNING;

                        sprintf(message_buffer, "%f - Process %d: end %s input", mic_to_sec(e_t), pcb->pid, descriptors[mdr->descriptor_id]);
                        log_with_config(message_buffer);
                        e_t = get_current_time() - simulator_start_time;
                        //printf("Error : %f ms\n", (e_t-s_t-t_args.duration)/1000.f);
                }
                else if(codes[mdr->code_id] == 'O')
                {
                    s_t = get_current_time() - simulator_start_time; 
                    sprintf(message_buffer, "%f - Process %d: start %s output", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid, descriptors[mdr->descriptor_id]);
                    log_with_config(message_buffer);
                    
                    pcb->process_state = WAITING;
                    pthread_join(start_output_thread(&t_args, pcb),NULL); // start an output thread
                    pcb->process_state = RUNNING;

                    sprintf(message_buffer, "%f - Process %d: end %s output", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid, descriptors[mdr->descriptor_id]);
                    log_with_config(message_buffer);
                    e_t = get_current_time() - simulator_start_time;
                   // printf("Error : %f ms\n", (e_t-s_t-t_args.duration)/1000.f);
                }
                else if(codes[mdr->code_id] == 'M')
                {
                    if(!strcmp(descriptors[mdr->descriptor_id], "block"))
                    {
                        s_t = get_current_time() - simulator_start_time; 
                        sprintf(message_buffer, "%f - Process %d: start memory blocking", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                        log_with_config(message_buffer);

                        pcb->process_state = WAITING;
                        block_memory(&t_args);
                        pcb->process_state = RUNNING;

                        //pthread_join(start_output_thread(&t_args, pcb),NULL);
                        sprintf(message_buffer, "%f - Process %d: end memory blocking", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                        log_with_config(message_buffer);
                        e_t = get_current_time() - simulator_start_time;
                       // printf("Error : %f ms\n", (e_t-s_t-t_args.duration)/1000.f);

                    }
                    if(!strcmp(descriptors[mdr->descriptor_id], "allocate"))
                    {
                        s_t = get_current_time() - simulator_start_time; 
                        sprintf(message_buffer, "%f - Process %d: allocating memory", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                        log_with_config(message_buffer);
                        //pthread_join(start_output_thread(&t_args, pcb),NULL);
                        unsigned int address = 0x0; 

                        pcb->process_state = WAITING;
                        allocate_memory(&t_args, &address, 0);
                        pcb->process_state = RUNNING;

                        sprintf(message_buffer, "%f - Process %d: memory allocated at %#010x", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid, address);
                        log_with_config(message_buffer);
                        e_t = get_current_time() - simulator_start_time;
                       // printf("Error : %f ms\n", (e_t-s_t-t_args.duration)/1000.f);
                    }    
                }
     
            }            
        }
    }
    if(!md->error_log.consumed)//print outstanding errors 
    { 
        print_log_message_with_config(&md->error_log);
    }
}
