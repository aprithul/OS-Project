#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char h[2048];

}log;

void foo(log* l,char* c)
{
    strcpy(l->h,c);    
}

void bar(log* l)
{
    char* x ="hello"; 
    foo(l, x);
}



int main(int argc, char* argv[])
{
    log l;
    strcpy(l.h ,"\0");
    printf("%d\n", (int)strlen(l.h));    
 

}
