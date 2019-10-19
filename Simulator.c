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
#include "Resource.h"
#include "CpuSchedulingCodes.h"
#include "Scheduler.h"

//j#define to_string(node) ((char*)(node->value))
//LinkedListmdb* construct_metadata(char* meta_data_text);
extern char codes[];
extern char* descriptors[];
extern int get_total_cycle_time(Configuration* config, MetaDataRecord* mdr);

Node* construct_node_int(Node* node, int val);
void calculate_remaining_time(Configuration* _config, MetaData* _md, int pid);
void* start_process_loader();
LinkedList process_queue;

PCB pcbs[4096];
MetaData metadatas[5];
unsigned long s_t,e_t;
Configuration config;
void execute_program_rr(MetaData* metadata, int pid_index);
void execute_program_srt(MetaData* md, int pid_index);
double simulator_start_time;
int simulator_complete = 0;
Resources resources;
double last_time = 0;
int interrupted = 0;
int last_pid = 0;
char message_buffer_interrupt[4096];

int get_srt_process()
{
    int _i = 0;
    int low = 9999999;
    int selected_p = 1;
    for(;_i<process_queue.count;_i++)
    {
        int _pid = *((int*)(ll_get(&(process_queue),_i)->value));
        if( metadatas[_pid-1].remainders[_pid-1] < low )
        {
            low = metadatas[_pid-1].remainders[_pid-1];
            selected_p = _pid;
        }

    }
    
    return selected_p;

}



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
        
        if(error_code == 0) // if there was no error in configuration file, read metadata file
        {
            initialize_resource_manager(&resources, 4, 2);

            config.constructed = 1; 
            //MetaData metadata;
            //error_code = construct_metadata(&metadata,config.md_file_path);
            // start the process loading thread 
            pthread_t t_id;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_create(&t_id, &attr, start_process_loader,NULL); 
            
            //while(process_queue.count == 0);//wait until at least one process is available 
            log_with_config("starting execution");
            simulator_start_time = get_current_time();
            last_time = simulator_start_time;
            sprintf(message_buffer, "%f - %s", mic_to_sec(get_current_time() - simulator_start_time),"Simulator program starting");
            log_with_config(message_buffer);
  
            // RR
            if(config.csc == RR)
            { 
                int cur_process = 0; 
                // call the executers
                while(!simulator_complete || process_queue.count>0)
                {
                    while(process_queue.count == 0);

                    //cur_process = (cur_process + 1) % process_queue.count;
                    int _pid = *((int*)(ll_get(&(process_queue),0)->value));
                    execute_program_rr( &metadatas[_pid-1], _pid-1);
                }
            }
            else if(config.csc == STR)
            {
                while(!simulator_complete || process_queue.count>0)
                {
                    while(process_queue.count == 0);
                    
                    int _pid = get_srt_process();
                    if(last_pid != _pid)
                    {
                        last_pid = _pid;
                        if(interrupted)
                        {
                            log_with_config(message_buffer_interrupt);
                            interrupted = 0;
                        }
                    }
                    execute_program_srt( &metadatas[_pid-1], _pid-1);
                }
            }
            //while(!simulator_complete);
            sprintf(message_buffer, "%f - %s", mic_to_sec(get_current_time() - simulator_start_time),"Simulator program ending");
            log_with_config(message_buffer);

            //printf("%d:error code \n", had_error);  
            //ll_print(&metadata.records); 
            //if(error_code>=0) // if metadata file returns <0 , it means either md file doesn't exist or file is empty. otherwise print the values
            {
                //printf("%d %d %d\n", metadata.process_indices[0], metadata.process_indices[1], metadata.process_indices[2]);
                //calculate_remaining_time(&config, &metadata, 1);
               
               //execute_program(&metadata);
              /*  
                return 0;
                if(config.csc == FIFO)
                   schedule_fifo(&metadata);
                else if(config.csc == SJF)
                   schedule_sjf(&metadata);
                else if(config.csc == PS)
                   schedule_ps(&metadata); 
                */
                
            } 
        }
        else
        {
            print_config(&config); // logs config file
        }    
        //construct_metadata("helo : world");   

    }
    else
    {
        log_detailed_to(LOGMODE_MONITOR, LOGTYPE_ERROR,0, NULL, "No configuration file specified", NULL);
        return -1; 
    }
    while(!simulator_complete);

    close_log_fp();//closes the log file before program exists
    return 0; 
}

void execute_program_srt(MetaData* md, int pid_index)
{
    //if(pid_index+1 == 3)
    //    printf("state %d \n", pcbs[pid_index].process_state);
    //
    if(pcbs[pid_index].process_state == WAITING || pcbs[pid_index].process_state == EXIT)    // if waiting for input/output can't 
    {
        
        md->process_remaining_times[pid_index] -= mic_to_milsec( get_current_time() - last_time);
        last_time = get_current_time();
        return;
    }
    else if(pcbs[pid_index].last_state == WAITING && pcbs[pid_index].process_state == RUNNING)
    {
        md->remainders[pid_index] = 0; 
        md->process_indices[md->process_id[pid_index]]++; //change starting index for process for next time process is active 
    } 

//    last_time = get_current_time();
    char message_buffer[512];
    PCB* pcb; 
    pcb = &(pcbs[pid_index]); 
    int process_id = 0;

    if(md->records.count>0)
    {
        //if(!strcmp(descriptors[mdr->descriptor_id], "begin"))
        srand(time(0));// set the seed for memory allocation 
        int p_l_i = 0;
        int p_h_i = 0;
        //int pid_index = 0;
        int _j = 0;
        //for(;_j<md->process_count;_j++)
        //    printf("%d,", md->process_id[_j]+1);
        //printf("\n");
        //for(; pid_index < md->process_count; pid_index++)
        {
            p_l_i = md->process_indices[md->process_id[pid_index]];
            p_h_i = md->process_id[pid_index] == md->process_count-1 ? (md->records).count : md->process_indices[md->process_id[pid_index]+1];
            //printf("%d --- %d\n", p_l_i, p_h_i);
            int i = md->process_indices[md->process_id[pid_index]]; //p_l_i;
        //    for(;i<p_h_i;i++)
        {
            
            MetaDataRecord* mdr = (MetaDataRecord*)(ll_get(&(md->records),i)->value);
            int cycle_time = md->remainders[pid_index] > 0 ? md->remainders[pid_index] : get_total_cycle_time(&config, mdr); // get the remaining cycle time if available
            //md->remainders[pid_index] = 0;
            /*if(quantum_size-cycle_time < 0)
            {
                md->remainders[pid_index] = -(quantum_size-cycle_time); // store absolute value of remaining cycle for next time process is active
                md->process_indices[md->process_id[pid_index]] = i; //change starting index for process for next time process is active 
                cycle_time = quantum_size;
                quantum_size = 0;
            }
            else
                quantum_size -= cycle_time;*/

            //md->process_indices[md->process_id[pid_index]] = i; //change starting index for process for next time process is active 
            md->t_args_s[pid_index].duration= cycle_time;//*1000;//cycle time is in ms. so convert it to micro s.
            md->t_args_s[pid_index].resource_name = descriptors[mdr->descriptor_id];
            md->t_args_s[pid_index].callback = NULL;

            md->io_t_args_s[pid_index].duration = get_total_cycle_time(&config, mdr)*1000;
            md->io_t_args_s[pid_index].resource_name = descriptors[mdr->descriptor_id];
            md->io_t_args_s[pid_index].callback = NULL;
            if(md->remainders[pid_index] == 0)
                md->remainders[pid_index] = cycle_time;
            //printf("%c\n", codes[mdr->code_id]);
            if(codes[mdr->code_id] == 'S')// && codes[mdr->code_id] != 'S')
            {
                
            }
            else if(codes[mdr->code_id] == 'A')// && codes[mdr->code_id] != 'S')
            {
                if(!strcmp(descriptors[mdr->descriptor_id], "begin"))
                {
                    process_id++;
                    process_id = md->process_id[pid_index]+1;
                    sprintf(message_buffer, "%f - %s %d", mic_to_sec(get_current_time() - simulator_start_time),"OS: preparing process",process_id);
                    log_with_config(message_buffer);
                    // create the process
                    // since the only thing we now have is the process state, assign appropriate value 
                    //pcb = malloc(sizeof(PCB));  // create pcb for current process 
                    pcb->process_state = START;   // assign state
                    pcb->pid = process_id;

                    md->io_t_args_s[pid_index].pcb = pcb;
                    md->t_args_s[pid_index].pcb = pcb;

                    // done creating process
                    pcb->process_state = READY; // assign state
                    md->process_indices[md->process_id[pid_index]]++; //change starting index for process for next time process is active 
                    sprintf(message_buffer, "%f - %s %d", mic_to_sec(get_current_time() - simulator_start_time),"OS: starting process",pcb->pid);
                    log_with_config(message_buffer);
                } 
                if(!strcmp(descriptors[mdr->descriptor_id], "finish"))
                {
                    int r_i = 0;
                    for(;r_i<process_queue.count; r_i++)
                        if(pcb->pid ==  *((int*)(ll_get(&(process_queue),r_i)->value)))
                            break;
                    int _x = 0;
                    //for(; _x<process_queue.count; _x++)
                    //    printf(" %d, ", *((int*)(ll_get(&(process_queue),_x)->value)));
                    //printf("\n");
                    //printf("remove %d\n", pcb->pid);
                    //printf("available %d\n", *((int*)(ll_get(&(process_queue),r_i)->value)));
                    //printf("%d\n", process_queue.count);
                    //printf("value at: %d  is %d \n", r_i, *((int*)(ll_get(&(process_queue),r_i)->value)));
                    ll_remove_at(&process_queue, r_i);
                    //printf("%d\n", process_queue.count);
                    sprintf(message_buffer, "%f - %s %d", mic_to_sec(get_current_time() - simulator_start_time),"OS: removing process", pcb->pid);
                    interrupted = 0;
                    log_with_config(message_buffer);
                    pcb->process_state = EXIT; // assign state
                    _x = 0;
                    //for(; _x<process_queue.count; _x++)
                    //    printf("p:  %d, ", *((int*)(ll_get(&(process_queue),_x)->value)));
                    //printf("\n");

                    //free(pcb);
                } 
 
            }
            else
            {   
                  
                pcb->process_state = RUNNING; // assign state
                if(codes[mdr->code_id] == 'P')
                {
                    if(!strcmp(descriptors[mdr->descriptor_id], "run"))
                    {
                        if(pcb->last_instruction_index != i)
                        {
                            sprintf(message_buffer, "%f - Process %d: start processing action", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                            log_with_config(message_buffer);
                            pcb->last_instruction_index = i;
                        }
                        //pcb->process_state = WAITING;
                        //set_timer_async(&(md->t_args_s[pid_index]));
                        //pcb->process_state = RUNNING;
                        md->process_remaining_times[pid_index] -= mic_to_milsec(get_current_time() - last_time);//cycle time is in ms. so convert it to micro s.
                        md->remainders[pid_index] -= mic_to_milsec( get_current_time() - last_time);
                      //  printf("%d  :  %lu\n", md->remainders[pid_index], md->t_args_s[pid_index].duration) ;

                        last_time = get_current_time();
                        interrupted = 1;
                        if(md->remainders[pid_index] <=0)
                        { 
                            interrupted = 0;
                            md->process_indices[md->process_id[pid_index]]++; //change starting index for process for next time process is active 
                            sprintf(message_buffer, "%f - Process %d: end processing action", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                            log_with_config(message_buffer);
                        }
                        //print if interrupted
                        sprintf(message_buffer_interrupt, "%f - Process %d: Interrupt processing action", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);

                    }
                }
                else if(codes[mdr->code_id] == 'I')
                {
                        md->remainders[pid_index] = 0; //no remainder if input
                    //    md->process_indices[md->process_id[pid_index]] = i+1; // input doesn't need to be resumed, so select next for startring index next time
                        
                        s_t = get_current_time() - simulator_start_time; 
                        sprintf(message_buffer, "%f - Process %d: start %s input", mic_to_sec(s_t), pcb->pid, descriptors[mdr->descriptor_id]);
                        if(!strcmp(descriptors[mdr->descriptor_id],"hard drive"))
                            sprintf(message_buffer, "%s on HDD %d", message_buffer, ((resources.hdd_count)++)%(resources.hdd_max_count));

                        log_with_config(message_buffer);

                        pcb->process_state = WAITING;
                        pcb->last_state = WAITING;
                        //pthread_join(start_input_thread(&t_args, descriptors[mdr->descriptor_id]),NULL);// start an input thread
                        start_input_thread(&(md->io_t_args_s[pid_index]));// start an input thread
                        //set_timer(&(md->t_args_s[pid_index]));
                        //pcb->process_state = RUNNING;

                        //sprintf(message_buffer, "%f - Process %d: end %s input", mic_to_sec(e_t), pcb->pid, descriptors[mdr->descriptor_id]);
                        //log_with_config(message_buffer);
                        e_t = get_current_time() - simulator_start_time;
                        //printf("Error : %f ms\n", (e_t-s_t-t_args.duration)/1000.f);
                }
                else if(codes[mdr->code_id] == 'O')
                {
                    md->remainders[pid_index] = 0; //no remainder if output
                   // md->process_indices[md->process_id[pid_index]] = i+1; // output doesn't need to be resumed, so select next for startring index next time
 
                    s_t = get_current_time() - simulator_start_time; 
                    sprintf(message_buffer, "%f - Process %d: start %s output", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid, descriptors[mdr->descriptor_id]);
                    if(!strcmp(descriptors[mdr->descriptor_id],"hard drive"))
                            sprintf(message_buffer, "%s on HDD %d", message_buffer, ((resources.hdd_count)++)%(resources.hdd_max_count));
                    else if(!strcmp(descriptors[mdr->descriptor_id],"projector"))
                            sprintf(message_buffer, "%s on PROJ %d", message_buffer, ((resources.projector_count)++)%(resources.projector_max_count));


                    log_with_config(message_buffer);
                    
                    pcb->process_state = WAITING;
                    pcb->last_state = WAITING;
                    start_output_thread(&(md->io_t_args_s[pid_index]));//, descriptors[mdr->descriptor_id], pcb->pid); // start an output thread
                    //pcb->process_state = RUNNING;

                    //sprintf(message_buffer, "%f - Process %d: end %s output", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid, descriptors[mdr->descriptor_id]);
                    //log_with_config(message_buffer);
                    e_t = get_current_time() - simulator_start_time;
                   // printf("Error : %f ms\n", (e_t-s_t-t_args.duration)/1000.f);
                }
                else if(codes[mdr->code_id] == 'M')
                {
                    if(!strcmp(descriptors[mdr->descriptor_id], "block"))
                    {
                        s_t = get_current_time() - simulator_start_time; 
                        
                        if(pcb->last_instruction_index != i)
                        {
                            sprintf(message_buffer, "%f - Process %d: start memory blocking", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                            log_with_config(message_buffer);
                            pcb->last_instruction_index = i;
                        } 
                        //pcb->process_state = WAITING;
                        //block_memory(&(md->t_args_s[pid_index]));
                        pcb->process_state = RUNNING;
                        
                        
                        md->process_remaining_times[pid_index] -= mic_to_milsec(get_current_time() - last_time);//cycle time is in ms. so convert it to micro s.
                        md->remainders[pid_index] -= mic_to_milsec( get_current_time() - last_time);
                        last_time = get_current_time();
                       
                        interrupted = 1; 
                        if(md->remainders[pid_index] <=0)
                        {
                            interrupted = 0;
                            //pthread_join(start_output_thread(&t_args, pcb),NULL);
                            md->process_indices[md->process_id[pid_index]]++; //change starting index for process for next time process is active 
                            sprintf(message_buffer, "%f - Process %d: end memory blocking", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                            log_with_config(message_buffer);
                        }
                        sprintf(message_buffer_interrupt, "%f - Process %d: Interrupt memory blocking", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                        e_t = get_current_time() - simulator_start_time;
                       // printf("Error : %f ms\n", (e_t-s_t-t_args.duration)/1000.f);

                    }
                    if(!strcmp(descriptors[mdr->descriptor_id], "allocate"))
                    {
                        s_t = get_current_time() - simulator_start_time; 
                        if(pcb->last_instruction_index != i)
                        {
                            sprintf(message_buffer, "%f - Process %d: allocating memory", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                            log_with_config(message_buffer);
                            pcb->last_instruction_index = i;
                        }
                        //pthread_join(start_output_thread(&t_args, pcb),NULL);
                        unsigned int address = 0x0; 

                        pcb->process_state = RUNNING;

                        md->process_remaining_times[pid_index] -= mic_to_milsec(get_current_time() - last_time);//cycle time is in ms. so convert it to micro s.
                        md->remainders[pid_index] -= mic_to_milsec( get_current_time() - last_time);
                        last_time = get_current_time();
                        interrupted = 1; 
                        if(md->remainders[pid_index] <=0)
                        {
                            interrupted = 0;

                            allocate_memory(&(md->t_args_s[pid_index]), &address, 0,1);
                            //pthread_join(start_output_thread(&t_args, pcb),NULL);
                            md->process_indices[md->process_id[pid_index]]++; //change starting index for process for next time process is active 

                            sprintf(message_buffer, "%f - Process %d: memory allocated at 0x%08x", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid, address);
                            log_with_config(message_buffer);
 
                        }
                        sprintf(message_buffer_interrupt, "%f - Process %d: Interrupt memory allocation", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                        e_t = get_current_time() - simulator_start_time;
                       // printf("Error : %f ms\n", (e_t-s_t-t_args.duration)/1000.f);
                    }    
                }
     
            }            
        }
        
        }
        
        //if(!strcmp(descriptors[mdr->descriptor_id], "finish"))
        
    }
    if(!md->error_log.consumed)//print outstanding errors 
    { 
        print_log_message_with_config(&md->error_log);
    }
}




void execute_program_rr(MetaData* md, int pid_index)
{
    //if(pid_index+1 == 3)
    //    printf("state %d \n", pcbs[pid_index].process_state);
    if(pcbs[pid_index].process_state == WAITING || pcbs[pid_index].process_state == EXIT)    // if waiting for input/output can't 
    {
        ll_remove_at(&process_queue, 0);
        Node node;
        ll_append(&process_queue, construct_node_int(&node, pid_index+1));

        return;
    }

    char message_buffer[512];
    PCB* pcb; 
    pcb = &(pcbs[pid_index]); 
    int process_id = 0;
    int quantum_size = config.quantum; 

    if(md->records.count>0)
    {
        //if(!strcmp(descriptors[mdr->descriptor_id], "begin"))
        srand(time(0));// set the seed for memory allocation 
        int p_l_i = 0;
        int p_h_i = 0;
        //int pid_index = 0;
        int _j = 0;
        //for(;_j<md->process_count;_j++)
        //    printf("%d,", md->process_id[_j]+1);
        //printf("\n");
        //for(; pid_index < md->process_count; pid_index++)
        {
            p_l_i = md->process_indices[md->process_id[pid_index]];
            p_h_i = md->process_id[pid_index] == md->process_count-1 ? (md->records).count : md->process_indices[md->process_id[pid_index]+1];
            //printf("%d --- %d\n", p_l_i, p_h_i);
            int i = p_l_i;
            for(;i<p_h_i;i++)
        {
            
            MetaDataRecord* mdr = (MetaDataRecord*)(ll_get(&(md->records),i)->value);
            int cycle_time = md->remainders[pid_index] > 0 ? md->remainders[pid_index] : get_total_cycle_time(&config, mdr); // get the remaining cycle time if available
            md->remainders[pid_index] = 0;
            if(quantum_size-cycle_time < 0)
            {
                md->remainders[pid_index] = -(quantum_size-cycle_time); // store absolute value of remaining cycle for next time process is active
                md->process_indices[md->process_id[pid_index]] = i; //change starting index for process for next time process is active 
                cycle_time = quantum_size;
                quantum_size = 0;
            }
            else
                quantum_size -= cycle_time;

            md->t_args_s[pid_index].duration= cycle_time*1000;//cycle time is in ms. so convert it to micro s.
            md->t_args_s[pid_index].resource_name = descriptors[mdr->descriptor_id];
            md->t_args_s[pid_index].callback = NULL;

            md->io_t_args_s[pid_index].duration = get_total_cycle_time(&config, mdr)*1000;
            md->io_t_args_s[pid_index].resource_name = descriptors[mdr->descriptor_id];
            md->io_t_args_s[pid_index].callback = NULL;

            //printf("%c\n", codes[mdr->code_id]);
            if(codes[mdr->code_id] == 'S')// && codes[mdr->code_id] != 'S')
            {
                
            }
            else if(codes[mdr->code_id] == 'A')// && codes[mdr->code_id] != 'S')
            {
                if(!strcmp(descriptors[mdr->descriptor_id], "begin"))
                {
                    process_id++;
                    process_id = md->process_id[pid_index]+1;
                    sprintf(message_buffer, "%f - %s %d", mic_to_sec(get_current_time() - simulator_start_time),"OS: preparing process",process_id);
                    log_with_config(message_buffer);
                    // create the process
                    // since the only thing we now have is the process state, assign appropriate value 
                    //pcb = malloc(sizeof(PCB));  // create pcb for current process 
                    pcb->process_state = START;   // assign state
                    pcb->pid = process_id;

                    md->io_t_args_s[pid_index].pcb = pcb;
                    md->t_args_s[pid_index].pcb = pcb;

                    // done creating process
                    pcb->process_state = READY; // assign state
                    sprintf(message_buffer, "%f - %s %d", mic_to_sec(get_current_time() - simulator_start_time),"OS: starting process",pcb->pid);
                    log_with_config(message_buffer);
                } 
                if(!strcmp(descriptors[mdr->descriptor_id], "finish"))
                {
                    int r_i = 0;
                    for(;r_i<process_queue.count; r_i++)
                        if(pcb->pid ==  *((int*)(ll_get(&(process_queue),r_i)->value)))
                            break;
                    int _x = 0;
                    //for(; _x<process_queue.count; _x++)
                    //    printf(" %d, ", *((int*)(ll_get(&(process_queue),_x)->value)));
                    //printf("\n");
                    //printf("remove %d\n", pcb->pid);
                    //printf("available %d\n", *((int*)(ll_get(&(process_queue),r_i)->value)));
                    //printf("%d\n", process_queue.count);
                    //printf("value at: %d  is %d \n", r_i, *((int*)(ll_get(&(process_queue),r_i)->value)));
                    ll_remove_at(&process_queue, r_i);
                    //printf("%d\n", process_queue.count);
                    sprintf(message_buffer, "%f - %s %d", mic_to_sec(get_current_time() - simulator_start_time),"OS: removing process", pcb->pid);
                    log_with_config(message_buffer);
                    pcb->process_state = EXIT; // assign state
                    _x = 0;
                    //for(; _x<process_queue.count; _x++)
                    //    printf("p:  %d, ", *((int*)(ll_get(&(process_queue),_x)->value)));
                    //printf("\n");

                    //free(pcb);
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
                        set_timer(&(md->t_args_s[pid_index]));
                        pcb->process_state = RUNNING;

                        if(md->remainders[pid_index] > 0) 
                        {
                            //printf("remainder %d\n", md->remainders[pid_index] );
                            sprintf(message_buffer, "%f - Process %d: Interrupt processing action", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                            log_with_config(message_buffer);
                            ll_remove_at(&process_queue, 0);
                            Node node;
                            ll_append(&process_queue, construct_node_int(&node, pcb->pid));
                            return;
                        }
                        else
                        { 
                            sprintf(message_buffer, "%f - Process %d: end processing action", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                            log_with_config(message_buffer);
                        }

                    }
                }
                else if(codes[mdr->code_id] == 'I')
                {
                        md->remainders[pid_index] = 0; //no remainder if input
                        md->process_indices[md->process_id[pid_index]] = i+1; // input doesn't need to be resumed, so select next for startring index next time
                        
                        s_t = get_current_time() - simulator_start_time; 
                        sprintf(message_buffer, "%f - Process %d: start %s input", mic_to_sec(s_t), pcb->pid, descriptors[mdr->descriptor_id]);
                        if(!strcmp(descriptors[mdr->descriptor_id],"hard drive"))
                            sprintf(message_buffer, "%s on HDD %d", message_buffer, ((resources.hdd_count)++)%(resources.hdd_max_count));

                        log_with_config(message_buffer);

                        pcb->process_state = WAITING;
                        //pthread_join(start_input_thread(&t_args, descriptors[mdr->descriptor_id]),NULL);// start an input thread
                        start_input_thread(&(md->io_t_args_s[pid_index]));// start an input thread
                        set_timer(&(md->t_args_s[pid_index]));
                        if(pcb->process_state == WAITING)
                        {
                            ll_remove_at(&process_queue, 0);
                            Node node;
                            ll_append(&process_queue, construct_node_int(&node, pcb->pid));
 
                            return; 
                        }
                        //pcb->process_state = RUNNING;

                        //sprintf(message_buffer, "%f - Process %d: end %s input", mic_to_sec(e_t), pcb->pid, descriptors[mdr->descriptor_id]);
                        //log_with_config(message_buffer);
                        e_t = get_current_time() - simulator_start_time;
                        //printf("Error : %f ms\n", (e_t-s_t-t_args.duration)/1000.f);
                }
                else if(codes[mdr->code_id] == 'O')
                {
                    md->remainders[pid_index] = 0; //no remainder if output
                    md->process_indices[md->process_id[pid_index]] = i+1; // output doesn't need to be resumed, so select next for startring index next time
 
                    s_t = get_current_time() - simulator_start_time; 
                    sprintf(message_buffer, "%f - Process %d: start %s output", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid, descriptors[mdr->descriptor_id]);
                    if(!strcmp(descriptors[mdr->descriptor_id],"hard drive"))
                            sprintf(message_buffer, "%s on HDD %d", message_buffer, ((resources.hdd_count)++)%(resources.hdd_max_count));
                    else if(!strcmp(descriptors[mdr->descriptor_id],"projector"))
                            sprintf(message_buffer, "%s on PROJ %d", message_buffer, ((resources.projector_count)++)%(resources.projector_max_count));


                    log_with_config(message_buffer);
                    
                    pcb->process_state = WAITING;
                    start_output_thread(&(md->io_t_args_s[pid_index]));//, descriptors[mdr->descriptor_id], pcb->pid); // start an output thread
                    set_timer(&(md->t_args_s[pid_index]));
                    if(pcb->process_state == WAITING)
                    {
                        ll_remove_at(&process_queue, 0);
                        Node node;
                        ll_append(&process_queue, construct_node_int(&node, pcb->pid));
 
                        return; 
                    }
                    //pcb->process_state = RUNNING;

                    //sprintf(message_buffer, "%f - Process %d: end %s output", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid, descriptors[mdr->descriptor_id]);
                    //log_with_config(message_buffer);
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
                        block_memory(&(md->t_args_s[pid_index]));
                        pcb->process_state = RUNNING;

                        if(md->remainders[pid_index] > 0) 
                        {
                            //pthread_join(start_output_thread(&t_args, pcb),NULL);
                            sprintf(message_buffer, "%f - Process %d: interrupt memory blocking", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                            log_with_config(message_buffer);
                            ll_remove_at(&process_queue, 0);
                            Node node;
                            ll_append(&process_queue, construct_node_int(&node, pcb->pid));
                            return;
                        }
                        else
                        {
                            //pthread_join(start_output_thread(&t_args, pcb),NULL);
                            sprintf(message_buffer, "%f - Process %d: end memory blocking", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                            log_with_config(message_buffer);
                        }
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
                        allocate_memory(&(md->t_args_s[pid_index]), &address, 0, 0);
                        pcb->process_state = RUNNING;

                        if(md->remainders[pid_index] > 0) 
                        {
                            sprintf(message_buffer, "%f - Process %d: memory allocation interrupted", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid);
                            log_with_config(message_buffer);
                            
                            ll_remove_at(&process_queue, 0);
                            Node node;
                            ll_append(&process_queue, construct_node_int(&node, pcb->pid));
 
                            return;
                        }
                        else
                        {
                            sprintf(message_buffer, "%f - Process %d: memory allocated at 0x%08x", mic_to_sec(get_current_time() - simulator_start_time), pcb->pid, address);
                            log_with_config(message_buffer);
 
                        }
                        e_t = get_current_time() - simulator_start_time;
                       // printf("Error : %f ms\n", (e_t-s_t-t_args.duration)/1000.f);
                    }    
                }
     
            }            
        }
        
        }
        
        //if(!strcmp(descriptors[mdr->descriptor_id], "finish"))
        
    }
    if(!md->error_log.consumed)//print outstanding errors 
    { 
        print_log_message_with_config(&md->error_log);
    }
}

void calculate_remaining_time(Configuration* _config, MetaData* _md, int pid)
{
    _md->process_remaining_times[pid-1] = 0;
    int _i = _md->process_indices[pid-1];
    int _j = _md->process_count == pid ? (_md->records).count - 1: _md->process_indices[pid];
    for(; _i < _j; _i++)
    {
        MetaDataRecord* mdr = (MetaDataRecord*)(ll_get(&(_md->records),_i)->value);
        int cycle_time = get_total_cycle_time(_config, mdr);
 
        _md->process_remaining_times[pid-1] += cycle_time; 
    }
}
// creates Node with mdb as it's value
Node* construct_node_int(Node* node, int val)
{
    node = malloc(sizeof(Node));
    node->value = malloc(sizeof(int));
    *((int*)(node->value)) = val;
    //memcpy(node->value, val, sizeof(int));
    return node;
}
void* start_process_loader()
{

    ll_construct(&process_queue, 10); // use list as queue to keep RR ordering

    int _i = 1;
    int _pc = 6;
    while((_i)<=_pc)
    {
        int error_code = construct_metadata(&metadatas[_i-1],config.md_file_path);
        if(error_code>=0)
        {
            calculate_remaining_time(&config, &metadatas[_i-1], _i);
            Node pid_node;
            ll_append(&process_queue, construct_node_int(&pid_node, _i));
            //printf("Process %d loaded\n", _i);
            _pc = metadatas[_i-1].process_count;
            pthread_t t_id;
            pthread_attr_t attr;
            pthread_attr_init(&attr);

            TimerArgs t_args;
            t_args.duration = 100*1000;//cycle time is in ms. so convert it to micro s.
            t_args.callback = NULL;

            pthread_create(&t_id, &attr, timer_countdown,&t_args); 
            pthread_join(t_id, NULL); 
            

            
        }
        else
            break;

        _i++;
    }
    simulator_complete = 1;
}


