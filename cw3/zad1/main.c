#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("[ERROR] Required parameters: n\n");
        return 0;
    }

    int n = atoi(argv[1]);

    printf("Main process PID: %d\n", (int)getpid());
    for (int i = 0; i < n; ++i)
    {
        if (fork() == 0) 
        {
            printf("Child process %d PID: %d\n", i, (int)getpid());    
            return 0;
        }
    }

    return 0;
}
