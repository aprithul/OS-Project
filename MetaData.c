#include "MetaData.h"
#include "Parser.h"
#include <stdio.h>
char* metadata_header = "Start Program Meta-Data Code:";
typedef struct
{
    char code;
    char* descriptor;
    int cycles;

}MetaDataBlock;
char* metadata_footer = "End Program Meta-Data Code.";

LinkedList construct_metadata(char* meta_data_text)
{
    printf("%s \n", meta_data_text);
    LinkedList linked_list;
    ll_construct(&linked_list,10);
    int split_length = 0; 
    char** splitted = split(meta_data_text,':', &split_length);   
    return linked_list;
}
