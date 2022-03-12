#include "library.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_COMMAND_LENGTH 1024

char** RESULTS;
int BLOCKS_COUNT;

void createResultsBlocks(int blocksCount)
{
    char** RESULTS = calloc(blocksCount, sizeof(char*));
    int BLOCKS_COUNT = blocksCount;
}

void gatherStats(char** filePaths, int filesCount, char* tempFilePath)
{
    char* command = calloc(MAX_COMMAND_LENGTH, sizeof(char));
    int status;

    for (int i = 0; i < filesCount; i++)
    {
        int commandLength = snprintf(command, MAX_COMMAND_LENGTH,
                                     "wc --lines --words --chars %s > %s", 
                                     filePaths[i], tempFilePath);
        if (commandLength < 0)
            printf("Encoding error has occured");
        else if (commandLength > MAX_COMMAND_LENGTH)
            printf("The command is too long");
        else
            status = system(command);

        loadStatsToMemory(tempFilePath);
    }
}

int loadStatsToMemory(char* tempFilePath)
{
    FILE* filePointer = fopen(tempFilePath, "r");
    if (filePointer == NULL) 
    {
        printf("Couldn't open the file %s", tempFilePath);
        return -1;
    }

    char* result = calloc(getFileSize(filePointer), sizeof(char));
    fscanf(filePointer, "%s", result);

    fclose(filePointer);

    return 0;
}

long getFileSize(FILE* filePointer)
{
    if (fseek(filePointer, 0, SEEK_END) == 0)
        return ftell(filePointer);

    return -1;
}
