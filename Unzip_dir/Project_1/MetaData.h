#ifndef METADATA_H
#define METADATA_H
#include "LinkedList.h"
#include "Configuration.h"
#include "Logger.h"
#include "Utils.h"
#include <unistd.h>

typedef struct
{
    int code_id;
    int descriptor_id;
    int cycles;

}MetaDataRecord;

typedef struct
{ 
    char* metadata_header;
    LinkedList records;      
    char* metadata_footer;
    
    LogMessage error_log;
}MetaData;
int construct_metadata(MetaData* metadata, char* metadata_file_name);
int get_metadata_descriptor_id(char* descriptor);
int get_metadata_code_id(char code);
void print_metadata(Configuration* config, MetaData* md);
Node* construct_node_mdb(Node* node, MetaDataRecord* mdr);
int get_total_cycle_time(Configuration* config,MetaDataRecord* mdr);
int record_is_valid(MetaDataRecord* mdr);
#endif
