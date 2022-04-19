#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_DATA_LINE_LEN 256
#define MAX_COMPONENTS_IN_LINE 32
#define COMPONENT_NAME_COMMON_PART_LEN 8
#define PIPE_READ 0
#define PIPE_WRITE 1

int componentsCnt, tasksCnt;

char** callocArray2DChar(int r, int c)
{
    char **A = (char **)malloc((unsigned int)r*sizeof(char *));
    for (int i = 0; i < r; i++)
        A[i] = (char *)calloc((unsigned int)c, sizeof(char));

    return A;
}

int** callocArray2DInt(int r, int c)
{
    int **A = (int **)malloc((unsigned int)r*sizeof(int *));
    for (int i = 0; i < r; i++)
        A[i] = (int *)calloc((unsigned int)c, sizeof(int));

    return A;
}

FILE* openFile(char* fileName, char* mode)
{
    FILE* fPtr = fopen(fileName, mode);
    if (fPtr == NULL)
    {
        printf("[ERROR] Couldn't open the file %s\n", fileName);
        exit(2);
    }

    return fPtr;
}

void initComponentsTasksCnt(FILE* fPtr)
{
    char* line = NULL; size_t len = 0; ssize_t read; int i = 0;
    while ((read = getline(&line, &len, fPtr)) != -1)
    {
        if (strcmp(line, "\n") == 0)
            componentsCnt = i;
        else
            ++i;
    }
    tasksCnt = i - componentsCnt;
    fseek(fPtr, 0, SEEK_SET);
}

void loadComponents(FILE* fPtr, char** components)
{
    char* line = NULL; size_t len = 0; ssize_t read; int i = 0;
    while ((read = getline(&line, &len, fPtr)) != -1)
    {
        components[i++] = strchr(line, '=') + 2;
        if (strcmp(line, "\n") == 0)
            break;
    }
}

// Call it after loadComponents
void loadTasks(FILE* fPtr, int** tasks, char** components)
{
    char* line = NULL; size_t len = 0; ssize_t read; int i = 0;
    char* temp;
    while ((read = getline(&line, &len, fPtr)) != -1)
    {
        int j = 1;
        temp = strtok(line, "| ");
        tasks[i][j++] = atoi(temp+COMPONENT_NAME_COMMON_PART_LEN+1);
        while ((temp = strtok(NULL, "| ")) != NULL)
            tasks[i][j++] = atoi(temp+COMPONENT_NAME_COMMON_PART_LEN+1);
        tasks[i][0] = j - 1;
        ++i;
    }
}

void runTasks(char** components, int** tasks)
{
    // for (int i = 0; i < tasksCnt; ++i)
    // {
    //     if (fork() == 0) // child
    //     {
    //         for (int j = 0; j < tasks[i][0]; ++j)
    //         {
    //             // fork();
    //         }
    //     }
    //     else // parent
    //         while (wait(NULL) > 0);
    // }
    // int fd[2];
    // pipe(fd);
    // pid_t pid = fork();
    // if (pid == 0) { // dziecko
    //     close(fd[1]); 
    //     // read(fd[0], ...) - odczyt danych z potoku
    // } else { // rodzic
    //     close(fd[0]);
    //     // write(fd[1], ...) - zapis danych do potoku
    // }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("[ERROR] Required parameters: filePath\n");
        return -1;
    }

    char* filePath = argv[1];

    char** components = NULL;
    int** tasks = NULL;
    FILE* fPtr = openFile(filePath, "r");
    initComponentsTasksCnt(fPtr);

    // components[i][0] = arguments count for the component
    // components[i][j] = jth argument of ith component (j > 0)
    components = callocArray2DChar(componentsCnt, MAX_DATA_LINE_LEN);
    loadComponents(fPtr, components);

    // tasks[i][0] = components count for the task
    // tasks[i][j] = jth componentId of ith task (j > 0)
    tasks = callocArray2DInt(tasksCnt, MAX_COMPONENTS_IN_LINE);
    loadTasks(fPtr, tasks, components);

    runTasks(components, tasks);

    return 0;
}
