#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_DATA_LINE_LEN 256
#define MAX_COMPONENTS_IN_LINE 32
#define MAX_COMPONENT_ARGS 8
#define MIN_COMPONENT_ID 1
#define COMPONENT_NAME_COMMON_PART_LEN 8
#define READ 0
#define WRITE 1

int componentsCnt, tasksCnt;

int** callocArray2DInt(int r, int c)
{
    int **A = (int **)malloc((unsigned int)r*sizeof(int *));
    for (int i = 0; i < r; i++)
        A[i] = (int *)calloc((unsigned int)c, sizeof(int));

    return A;
}

void freeArray3DChar(char ***A, int r, int c)
{
    for (int i = 0; i < r; i++)
    {
        // ??? should work
        // int j = 0;
        // while (j < c && A[i][j] != NULL)
        //     free(A[i][j++]);
        free(A[i]);
    }
    free(A);
}

void freeArray2DChar(char **A, int r)
{
    for (int i = 0; i < r; i++)
        free(A[i]);
    free(A);
}

void freeArray2DInt(int **A, int r)
{
    for (int i = 0; i < r; i++)
        free(A[i]);
    free(A);
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
        if (strcmp(line, "\n") == 0)
            break;
        int strLen = strlen(line);
        if (strLen > 0 && line[strLen-1] == '\n')
            line[strLen-1] = '\0';
        components[i++] = strdup(strchr(line, '=') + 2);
    }
}

// call it after loadComponents
void loadTasks(FILE* fPtr, int** tasks)
{
    char* line = NULL; size_t len = 0; ssize_t read; int i = 0;
    while ((read = getline(&line, &len, fPtr)) != -1)
    {
        int j = 1;
        char* temp = strtok(line, "| ");
        tasks[i][j++] = atoi(temp+COMPONENT_NAME_COMMON_PART_LEN+1);
        while ((temp = strtok(NULL, "| ")) != NULL)
            tasks[i][j++] = atoi(temp+COMPONENT_NAME_COMMON_PART_LEN+1);
        tasks[i][0] = j - 1;
        ++i;
    }
}

// split component on '|'
char** parseComponent(char* component)
{
    char** subcomponents = (char **)calloc((unsigned int)MAX_COMPONENTS_IN_LINE, sizeof(char *));
    char* temp = strdup(component);
    temp = strtok(temp, "|");
    int i = 0;
    subcomponents[i++] = temp;
    while ((temp = strtok(NULL, "|")) != NULL)
    {
        while (*temp == ' ')
            ++temp;
        subcomponents[i++] = temp;
    }
    return subcomponents;
}

// split subcomponent on ' '
char** parseSubcomponent(char* subcomponent)
{
    char** args = (char **)calloc((unsigned int)MAX_COMPONENT_ARGS, sizeof(char *));
    char* temp = strdup(subcomponent);
    temp = strtok(subcomponent, " ");
    int i = 0;
    args[i++] = temp;
    while ((temp = strtok(NULL, " ")) != NULL)
        args[i++] = temp;
    args[i] = NULL;
    return args;
}

int createFlatTaskArray(char** components, int** tasks, int taskId, char*** task)
{
    int flatTaskId = 0;
    for (int progId = 1; progId <= tasks[taskId][0]; ++progId)
    {
        int componentId = tasks[taskId][progId] - MIN_COMPONENT_ID;
        if (componentId == -1)
            break;

        printf("\tprogram id: %d, component id: %d\n", progId, componentId);
        
        char** subcomponents = parseComponent(components[componentId]);
        int i = 0;
        while (subcomponents[i] != NULL)
        {
            printf("\t\tsubcomponent id: %d, body: %s\n", i, subcomponents[i]);
            i++;
        }
        
        for (int subcomponentId = 0; subcomponentId < MAX_COMPONENTS_IN_LINE; ++subcomponentId)
        {
            if (subcomponents[subcomponentId] == NULL)
                break;
            char** args = parseSubcomponent(subcomponents[subcomponentId]);
            task[flatTaskId++] = args;
        }
    }
    return flatTaskId;
}

void runTask(char*** task, int taskLen)
{
    int pipes[MAX_COMPONENTS_IN_LINE][2];
    for (int i = 0; i < taskLen-1; ++i)
        pipe(pipes[i]);

    for (int i = 0; i < taskLen; ++i)
    {
        if (fork() == 0) 
        {
            if (i != taskLen - 1)
                dup2(pipes[i][WRITE], STDOUT_FILENO);

            if (i != 0)
                dup2(pipes[i-1][READ], STDIN_FILENO);

            for (int j = 0; j < taskLen-1; ++j)
            {
                close(pipes[j][READ]);
                close(pipes[j][WRITE]);
            }

            execvp(task[i][0], task[i]);
            exit(0);
        }
    }

    for (int i = 0; i < taskLen-1; ++i)
        close(pipes[i][WRITE]);

    while (wait(NULL) > 0);
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("[ERROR] Required parameters: filePath\n");
        return -1;
    }

    char* filePath = argv[1];

    FILE* fPtr = openFile(filePath, "r");
    initComponentsTasksCnt(fPtr);
    char** components = (char **)malloc((unsigned int)componentsCnt*sizeof(char *));
    int** tasks = NULL;

    loadComponents(fPtr, components);

    // tasks[i][0] = components count for the task
    // tasks[i][j] = jth componentId of ith task (j > 0)
    tasks = callocArray2DInt(tasksCnt, MAX_COMPONENTS_IN_LINE);
    loadTasks(fPtr, tasks);

    for (int taskId = 0; taskId < tasksCnt; ++taskId)
    {
        printf("> task id: %d\n", taskId);
        char*** task = (char ***)calloc((unsigned int)MAX_COMPONENTS_IN_LINE, sizeof(char **));
        int taskLen = createFlatTaskArray(components, tasks, taskId, task);
        printf("< OUTPUT\n");
        runTask(task, taskLen);
        printf("\n");
        freeArray3DChar(task, taskLen, MAX_COMPONENT_ARGS);
    }

    freeArray2DChar(components, componentsCnt);
    freeArray2DInt(tasks, tasksCnt);

    return 0;
}
