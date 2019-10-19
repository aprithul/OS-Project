#include <stdio.h>
#include <limits.h>
#include "MetaData.h"
#include "Utils.h"


char codes[] = {'S','A','P','I','O','M'};
char* descriptors[] = { "begin", "finish", "hard drive", "keyboard", "scanner", "monitor", "run", "allocate", "projector", "block"};

// construct_metadata Error codes:
//      1 = Error, will create partial Metadata
//      0 = No Error
//     -1 = Fatal Error, returning immediately

// constructor
int construct_metadata(MetaData* metadata, char* metadata_file_path)
{
    char message_buffer[512]; 
    int error_code = 0;
    ll_construct(&metadata->records,100);
    metadata->error_log.consumed = 1;
   
    // file doesn't exist 
    if(access(metadata_file_path, F_OK) == -1)
    {
        sprintf(message_buffer, "file not found : %s", metadata_file_path); 
        //set_error_log(&metadata->error_log,0,metadata_file_path, message_buffer);        
        log_detailed_with_config(LOGTYPE_ERROR, 0, NULL, message_buffer); 
        error_code = -1;
    }
    else
    {
        set_value_i(metadata->remainders, 4096, 0);
        char* metadata_text = read_file(metadata_file_path);
        if(!is_text_whitespace(metadata_text))// make sure read text isn't just whitespace
        {
            int split_len_newline = 0; 
            char del[] = {'\n','\0'};
            char** split_by_newline = split(metadata_text, del, &split_len_newline);  
            //printf("%d\n", split_len_newline); 
            int text_len = strlen(split_by_newline[split_len_newline-2]);
            char* header = strip(split_by_newline[0]); 
            char* footer = strip(split_by_newline[split_len_newline-1]); 
            if(strcmp(header, "Start Program Meta-Data Code:"))
            {
                set_error_log(&metadata->error_log,1,metadata_file_path, "No Start Program statement in Meta-Data file");        
                error_code = 1;
            }         
            else if(strcmp(footer, "End Program Meta-Data Code."))
            {
                set_error_log(&metadata->error_log,split_len_newline,metadata_file_path, "No End Program statement in Meta-Data file");        
                error_code = 1;
            }
            else if(split_by_newline[split_len_newline-2][text_len-1] != '.')//last line of md (before closign statement) should have an ending '.'
            {
                set_error_log(&metadata->error_log,split_len_newline-2,metadata_file_path, "Missing '.'");        
                error_code = 1; 
            }
            else
            {
                metadata->process_count = 0;
                split_by_newline[split_len_newline-2][text_len-1]=' '; // get rid of the '.', last meta data record will fail to read
                int i=1;
                split_by_newline[i] = strip(split_by_newline[i]);
                for( ; i<split_len_newline-1 && !error_code; i++)
                {
                    int split_len_sc = 0;
                    del[0] = ';';
                    
                    // see if there is a semicolon at the end of line 
                    //
                    split_by_newline[i] = strip(split_by_newline[i]);
                    int line_len = strlen(split_by_newline[i]);
                    if(split_by_newline[i][line_len-1]!=';' && i!=split_len_newline-2)
                    {
                        split_by_newline[i+1] = strip(split_by_newline[i+1]);
                        // or the start of next line
                        if(split_by_newline[i+1][0]!=';')
                        {
                            // we have an error otherwise
                            set_error_log(&metadata->error_log,i,metadata_file_path, "missing ';'");        
                            error_code = 1;
                        }
                    } 

                    char** split_by_sc = split(split_by_newline[i], del, &split_len_sc);
                    int j=0;
                    // split_by_sc items are in format code{descriptor}cycles 
                    // first we split it into code and descriptor}cycles
                    // then split the second part into  descriptor and cycles
                    // each split should generate a char** with two items 
                    for(;j<split_len_sc && !error_code;j++)
                    {
                        int split_len_lcb = 0;
                        del[0] = '{';
                        char** split_by_lcb = split(split_by_sc[j], del, &split_len_lcb);// first split by '{'
                        int code_id = -1;
                        
                        if(split_len_lcb == 2 && (code_id = get_metadata_code_id( strip(split_by_lcb[0])[0]))!=-1)
                        {
                           // printf("%s\n",strip(split_by_lcb[0]));
                            int split_len_rcb = 0;
                            del[0] = '}';
                            char** split_by_rcb = split(split_by_lcb[1], del, &split_len_rcb);//second split by '};
                            int descriptor_id = -1;
                            if(split_len_rcb == 2 && (descriptor_id = get_metadata_descriptor_id( strip(split_by_rcb[0])))!=-1)
                            {
                                if(code_id == 1 && descriptor_id == 0) // A{begin}
                                {
                                    metadata->process_task_counts[metadata->process_count] = 0;
                                    metadata->process_priorities[metadata->process_count] = 0;
                                    metadata->process_id[metadata->process_count] = metadata->process_count;
                                    metadata->process_count++;
                               }
                                char* temp = NULL;
                                long int cycles = strtol(strip(split_by_rcb[1]), &temp, 10);
                                if(cycles <0 || cycles > INT_MAX || *temp != '\0') // cycle value shouldn't be negative or greater than INT_MAX, temp anything other than '\0' means error while parsing to int
                                {
                                    sprintf(message_buffer, "couldn't parse: '%s'", split_by_sc[j]);
                                    set_error_log( &(metadata->error_log), (i+1), metadata_file_path, message_buffer);
                                    error_code = 1; 
                                }
                                else
                                {
                                    MetaDataRecord md_record;
                                    md_record.code_id = code_id;
                                    md_record.descriptor_id = descriptor_id;
                                    md_record.cycles = (int)cycles;// create a metadata record
                                    if(record_is_valid(&md_record))//only certain combination of code and descriptor are allowed
                                    {
                                        Node* node;
                                        ll_append(&metadata->records, construct_node_mdb(node, &md_record));// put the record in the records linked list inside metadata 
                                        if(descriptor_id>=2)
                                            metadata->process_task_counts[metadata->process_count-1]++;
                                        if(code_id == 3 || code_id == 4)
                                            metadata->process_priorities[metadata->process_count-1]++;
                                        if(code_id == 1 && descriptor_id == 0) // A{begin}
                                            metadata->process_indices[metadata->process_count-1] = (metadata->records).count-1;
                                    }
                                    else
                                    {
                                        error_code = 1;
                                        sprintf(message_buffer, "Not valid MetaData Code-Descriptor combination: '%c and %s'", codes[code_id],descriptors[descriptor_id]);
                                        set_error_log( &(metadata->error_log), (i+1), metadata_file_path, message_buffer);
                                     
                                    }
    //                              construct_node_mdb(node, &md_record);
                                }
                            }
                            else
                            {
                                sprintf(message_buffer, "couldn't parse: '%s'", split_by_sc[j]);
                                set_error_log( &(metadata->error_log), (i+1), metadata_file_path, message_buffer);
                                error_code = 1;
                            }
                            free_split_text(split_by_rcb, split_len_rcb);
                        }
                        else
                        {
                            sprintf(message_buffer, "couldn't parse: '%s'", split_by_sc[j]);
                            set_error_log( &(metadata->error_log), (i+1), metadata_file_path, message_buffer);
                            error_code = 1;
                        }
                        free_split_text(split_by_lcb, split_len_lcb);
                    } 
                    free_split_text(split_by_sc, split_len_sc);
                }
            }
            
            free_split_text(split_by_newline, split_len_newline);
        }
        else
        {
            //set_error_log( &(metadata->error_log), 0, metadata_file_path, "Metadata file is empty");
            sprintf(message_buffer, "%s : file is empty", metadata_file_path);
            log_with_config(message_buffer); 
            error_code = -1;
        }
    }
    
        
    return error_code; 
}

//prints records with total times
void print_metadata(Configuration* config, MetaData* md)
{
    char message_buffer[512];
    if(md->records.count>0)
    {
        log_with_config("Meta-Data Metrics");
        int i=0;
        for(;i<md->records.count;i++)
        {
            MetaDataRecord* mdr = (MetaDataRecord*)(ll_get(&(md->records),i)->value);
            if(codes[mdr->code_id] != 'A' && codes[mdr->code_id] != 'S')
            {
                sprintf(message_buffer,"%c{%s}%d - %d ms", codes[mdr->code_id], descriptors[mdr->descriptor_id], mdr->cycles, get_total_cycle_time(config,mdr));
                log_with_config(message_buffer); 
            }
        }
    }
    if(!md->error_log.consumed)//print outstanding errors 
    { 
        print_log_message_with_config(&md->error_log);
    }
}

// gets index in code array
int get_metadata_code_id(char code)
{
    int i = 0;
    for(;i<6;i++)
        if(codes[i] == code)
            return i;
    return -1;    
}

// gets index in descriptor array
int get_metadata_descriptor_id(char* descriptor)
{
    int i = 0;
    for(; i<10; i++)
    {
        if(!strcmp(descriptor, descriptors[i]))
            return i;
    } 
    return -1;
}

// creates Node with mdb as it's value
Node* construct_node_mdb(Node* node, MetaDataRecord* mdb)
{
    node = malloc(sizeof(Node));
    node->value = malloc(sizeof(MetaDataRecord));
    memcpy(node->value, mdb, sizeof(MetaDataRecord));
    return node;
}

//calculate and return total time calculated by multiplying cycles and each cycle time 
int get_total_cycle_time(Configuration* config, MetaDataRecord* mdr)
{
    if(codes[mdr->code_id] == 'P')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "run"))
            return config->processor_cycle_time * mdr->cycles;
        else
            return 0;
    }
    else if(codes[mdr->code_id] == 'I')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "hard drive"))
            return config->hard_drive_cycle_time * mdr->cycles;
        if(!strcmp(descriptors[mdr->descriptor_id], "keyboard"))
            return config->keyboard_cycle_time * mdr->cycles;
        if(!strcmp(descriptors[mdr->descriptor_id], "scanner"))
            return config->scanner_cycle_time * mdr->cycles;
        return 0;
    }
    else if(codes[mdr->code_id] == 'O')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "hard drive"))
            return config->hard_drive_cycle_time * mdr->cycles;
        if(!strcmp(descriptors[mdr->descriptor_id], "monitor"))
            return config->monitor_display_time * mdr->cycles;
        if(!strcmp(descriptors[mdr->descriptor_id], "projector"))
            return config->projector_cycle_time * mdr->cycles;
        return 0;
    }
    else if(codes[mdr->code_id] == 'M')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "block"))
            return config->memory_cycle_time * mdr->cycles;
        if(!strcmp(descriptors[mdr->descriptor_id], "allocate"))
            return config->memory_cycle_time * mdr->cycles;
        return 0;
    }
    return 0;
    
}

// is the code and descriptor combination valid?
int record_is_valid( MetaDataRecord* mdr)
{
    if(codes[mdr->code_id] == 'S')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "begin"))
            return 1;
        if(!strcmp(descriptors[mdr->descriptor_id], "finish"))
            return 1;
    }
    else if(codes[mdr->code_id] == 'A')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "begin"))
            return 1;
        if(!strcmp(descriptors[mdr->descriptor_id], "finish"))
            return 1;   
    }
    else if(codes[mdr->code_id] == 'P')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "run"))
            return 1;
    }
    else if(codes[mdr->code_id] == 'I')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "hard drive"))
            return 1; 
        if(!strcmp(descriptors[mdr->descriptor_id], "keyboard"))
            return 1;
        if(!strcmp(descriptors[mdr->descriptor_id], "scanner"))
            return 1;
    }
    else if(codes[mdr->code_id] == 'O')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "hard drive"))
            return 1;
        if(!strcmp(descriptors[mdr->descriptor_id], "monitor"))
            return 1;
        if(!strcmp(descriptors[mdr->descriptor_id], "projector"))
            return 1;
    }
    else if(codes[mdr->code_id] == 'M')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "block"))
            return 1;
        if(!strcmp(descriptors[mdr->descriptor_id], "allocate"))
            return 1;
    }
    return 0;
}
