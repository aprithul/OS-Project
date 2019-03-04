#include <stdio.h>
#include <unistd.h>

int main(int argc, int* argv[])
{
    int i =0;
    for(;i<2;i++)
    {
        fork();
        int pid = getpid();
        printf("%d\n", pid);
    }
    return 0;

}
