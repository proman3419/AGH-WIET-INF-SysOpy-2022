#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_DATA_LINE_LEN 256
#define MAX_COMPONENTS_IN_LINE 32
#define MAX_COMPONENT_ARGS 8
#define MAX_ARG_LEN 128
#define MIN_COMPONENT_ID 1
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
        if (strcmp(line, "\n") == 0)
            break;
        components[i++] = strdup(strchr(line, '=') + 2);
    }
}

// Call it after loadComponents
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
        subcomponents[i++] = temp;
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
    {
        // printf("%s\n", temp);
        args[i++] = temp;
    }
    return args;
}

void runTasks(char** components, int** tasks)
{
    for (int taskId = 0; taskId < tasksCnt; ++taskId)
    {
        for (int progId = 1; progId <= tasks[taskId][0]; ++progId)
        {
            int componentId = tasks[taskId][progId] - MIN_COMPONENT_ID;
            if (componentId == -1)
                break;
            for (int subcomponentId = 0; subcomponentId < MAX_COMPONENTS_IN_LINE; ++subcomponentId)
            {
                if (subcomponents[subcomponentId] == NULL)
                    break;
                char** args = parseSubcomponent(subcomponents[subcomponentId]);
            }
        }
    }
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

    // components[i][0] = arguments count for the component
    // components[i][j] = jth argument of ith component (j > 0)
    loadComponents(fPtr, components);

    // tasks[i][0] = components count for the task
    // tasks[i][j] = jth componentId of ith task (j > 0)
    tasks = callocArray2DInt(tasksCnt, MAX_COMPONENTS_IN_LINE);
    loadTasks(fPtr, tasks);

    runTasks(components, tasks);

    return 0;
}
