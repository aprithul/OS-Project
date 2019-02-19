#include "MetaData.h"
#include "Parser.h"
#include "Utils.h"
#include <stdio.h>
#include "Configuration.h"
#include <limits.h>

char codes[] = {'S','A','P','I','O','M'};
char* descriptors[] = { "begin", "finish", "hard drive", "keyboard", "scanner", "monitor", "run", "allocate", "projector", "block"};
int construct_metadata(MetaData* metadata, char* metadata_file_path)
{
    char message_buffer[512]; 
    int error_found = 0;
    if(access(metadata_file_path, F_OK) == -1)
    {
        sprintf(message_buffer, "%s:file not found", metadata_file_path); 
        set_error_log(&metadata->error_log,0,metadata_file_path, message_buffer);        
        error_found = 1;
    }
    else
    {
        metadata->error_log.consumed = 1;
        char* metadata_text = read_file(metadata_file_path);
        if(!is_text_whitespace(metadata_text))
        {
            ll_construct(&metadata->records,100);
            int split_len_newline = 0; 
            char del[] = {'\n','\0'};
            char** split_by_newline = split(metadata_text, del, &split_len_newline);   
            //printf("%d\n", split_len_newline); 
            int text_len = strlen(split_by_newline[split_len_newline-2]);
            char* header = strip(split_by_newline[0]); 
            if(strcmp(header, "Start Program Meta-Data Code:"))
            {
                set_error_log(&metadata->error_log,1,metadata_file_path, "Start statement not present in Meta-Data file");        
                error_found = 1;
            }         
            else if(split_by_newline[split_len_newline-2][text_len-1] != '.')
            {
                set_error_log(&metadata->error_log,split_len_newline-2,metadata_file_path, "Missing '.'");        
                error_found = 1; 
            }
            else
            {
                split_by_newline[split_len_newline-2][text_len-1]=' '; // get rid of full stop at the end of metadata
                int i=1;
                split_by_newline[i] = strip(split_by_newline[i]);
                for( ; i<split_len_newline-1 && !error_found; i++)
                {
                    int split_len_sc = 0;
                    del[0] = ';';
                    
                    // see if there is a missing semicolon at the end of line 
                    int line_len = strlen(split_by_newline[i]);
                    if(split_by_newline[i][line_len-1]!=';' && i!=split_len_newline-2)
                    {
                        split_by_newline[i+1] = strip(split_by_newline[i+1]);
                        if(split_by_newline[i+1][0]!=';')
                        {
                            set_error_log(&metadata->error_log,i,metadata_file_path, "missing ';'");        
                            error_found = 1;
                        }
                    } 

                    char** split_by_sc = split(split_by_newline[i], del, &split_len_sc);
                    int j=0;
                    
                    for(;j<split_len_sc && !error_found;j++)
                    {
                        int split_len_lcb = 0;
                        del[0] = '{';
                        char** split_by_lcb = split(split_by_sc[j], del, &split_len_lcb);
                        int code_id = -1;
                        
                        if(split_len_lcb == 2 && (code_id = get_metadata_code_id( strip(split_by_lcb[0])[0]))!=-1)
                        {
                            int split_len_rcb = 0;
                            del[0] = '}';
                            char** split_by_rcb = split(split_by_lcb[1], del, &split_len_rcb);
                            int descriptor_id = -1;
                            if(split_len_rcb == 2 && (descriptor_id = get_metadata_descriptor_id( strip(split_by_rcb[0])))!=-1)
                            {
                                char* temp = NULL;
                                long int cycles = strtol(strip(split_by_rcb[1]), &temp, 10);
                                if(cycles <0 || cycles > INT_MAX || *temp != '\0')
                                {
                                    sprintf(message_buffer, "couldn't parse: '%s'", split_by_sc[j]);
                                    set_error_log( &(metadata->error_log), (i+1), metadata_file_path, message_buffer);
                                    error_found = 1; 
                                }
                                else
                                {
                                    MetaDataRecord md_record;
                                    md_record.code_id = code_id;
                                    md_record.descriptor_id = descriptor_id;
                                    md_record.cycles = (int)cycles;
                                    if(record_is_valid(&md_record))
                                    {
                                        Node* node;
                                        ll_append(&metadata->records, construct_node_mdb(node, &md_record)); 
                                    }
                                    else
                                    {
                                        error_found = 1;
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
                                error_found = 1;
                            }
                            free_split_text(split_by_rcb, split_len_rcb);
                        }
                        else
                        {
                            sprintf(message_buffer, "couldn't parse: '%s'", split_by_sc[j]);
                            set_error_log( &(metadata->error_log), (i+1), metadata_file_path, message_buffer);
                            error_found = 1;
                        }
                        free_split_text(split_by_lcb, split_len_lcb);
                    } 
                    free_split_text(split_by_sc, split_len_sc);
                }
            }
            char* footer = strip(split_by_newline[split_len_newline-1]); 
            if(strcmp(footer, "End Program Meta-Data Code."))
            {
                set_error_log(&metadata->error_log,1,metadata_file_path, "End statement not present in Meta-Data file");        
                error_found = 1;
            }
            free_split_text(split_by_newline, split_len_newline);
        }
        else
        {
            set_error_log( &(metadata->error_log), 0, metadata_file_path, "Metadata file is empty");
            error_found = 1;
        }
    }
    
        
    return error_found; 
}

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
    if(!md->error_log.consumed)  
        print_log_message_with_config(&md->error_log);
}


int get_metadata_code_id(char code)
{
    int i = 0;
    for(;i<6;i++)
        if(codes[i] == code)
            return i;
    return -1;    
}

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

Node* construct_node_mdb(Node* node, MetaDataRecord* mdb)
{
    node = malloc(sizeof(Node));
    node->value = malloc(sizeof(MetaDataRecord));
    memcpy(node->value, mdb, sizeof(MetaDataRecord));
    return node;
}

int get_total_cycle_time(Configuration* config, MetaDataRecord* mdr)
{
    if(codes[mdr->code_id] == 'P')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "run"))
            return config->processor_cycle_time * mdr->cycles;
        else
            return -1;
    }
    else if(codes[mdr->code_id] == 'I')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "hard drive"))
            return config->hard_drive_cycle_time * mdr->cycles;
        if(!strcmp(descriptors[mdr->descriptor_id], "keyboard"))
            return config->keyboard_cycle_time * mdr->cycles;
        if(!strcmp(descriptors[mdr->descriptor_id], "scanner"))
            return config->scanner_cycle_time * mdr->cycles;
        return -1;
    }
    else if(codes[mdr->code_id] == 'O')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "hard drive"))
            return config->hard_drive_cycle_time * mdr->cycles;
        if(!strcmp(descriptors[mdr->descriptor_id], "monitor"))
            return config->monitor_display_time * mdr->cycles;
        if(!strcmp(descriptors[mdr->descriptor_id], "projector"))
            return config->projector_cycle_time * mdr->cycles;
        return -1;
    }
    else if(codes[mdr->code_id] == 'M')
    {
        if(!strcmp(descriptors[mdr->descriptor_id], "block"))
            return config->memory_cycle_time * mdr->cycles;
        if(!strcmp(descriptors[mdr->descriptor_id], "allocate"))
            return config->memory_cycle_time * mdr->cycles;
        return -1;
    }
    return -1;
    
}

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
