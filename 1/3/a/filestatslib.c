#include "filestatslib.h"
#include "loglib.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <time.h>

#define MAX_COMMAND_LENGTH 1024

char** BLOCKS;
int BLOCKS_COUNT;

void createBlocks(int blocksCount)
{
    if (BLOCKS != NULL)
        freeAllBlocks();
    BLOCKS = calloc(blocksCount, sizeof(char*));
    BLOCKS_COUNT = blocksCount;
    if (logInfoH()) 
        printf("Created %d blocks\n", BLOCKS_COUNT);
}

void gatherStats(char** filePaths, int filesCount, const char* tempFilePath)
{
    for (int i = 0; i < filesCount; i++)
    {
        usewc(filePaths[i], tempFilePath);
        loadFileToMemory(tempFilePath);
    }
}

void usewc(char* filePath, const char* tempFilePath)
{
    char* command = calloc(MAX_COMMAND_LENGTH, sizeof(char));
    int status, commandLength;

    commandLength = snprintf(command, MAX_COMMAND_LENGTH,
                             "wc --lines --words --chars %s > %s", 
                             filePath, tempFilePath);

    if (commandLength < 0)
        logError("Encoding error occurred for the command\n");
    else if (commandLength > MAX_COMMAND_LENGTH)
        logError("The command is too long\n");
    else
    {
        status = system(command);

        if (status == -1)
            if (logErrorH())
                printf("Failed to execute the command '%s'\n", command);
    }

    free(command);
}

int loadFileToMemory(const char* filePath)
{
    FILE* filePointer = fopen(filePath, "r");
    if (filePointer == NULL) 
    {
        if (logErrorH()) 
            printf("Couldn't open the file %s\n", filePath);
        return -1;
    }

    int fileSize = getFileSize(filePointer);
    char* result = calloc(fileSize, sizeof(char));
    fseek(filePointer, 0, SEEK_SET);
    fread(result, 1, fileSize, filePointer);
    fclose(filePointer);

    for (int i = 0; i < BLOCKS_COUNT; i++)
    {
        if (BLOCKS[i] == NULL)
        {
            BLOCKS[i] = result;
            if (logSuccessH()) 
                printf("Content of %s loaded at the block with id %d\n", filePath, i);
            return i;
        }
    }

    logError("No free space in the BLOCKS array\n");

    return -1;
}

void freeBlock(int blockId)
{
    free(BLOCKS[blockId]);
    BLOCKS[blockId] = NULL;
    if (logInfoH()) 
        printf("Removed the block with id %d\n", blockId);
}

long getFileSize(FILE* filePointer)
{
    if (fseek(filePointer, 0, SEEK_END) == 0)
        return ftell(filePointer);

    if (logErrorH()) 
        printf("Couldn't get the file size for %p\n", filePointer);

    return -1;
}

void printBlock(int blockId)
{
    if (BLOCKS == NULL)
        logError("The BLOCKS array is NULL, call createBlocks first\n");
    else if (blockId < 0 || BLOCKS_COUNT <= blockId)
    {
        if (logErrorH()) 
            printf("Block id should be from range [0, %d], was %d\n", BLOCKS_COUNT, blockId);
    }
    else if (BLOCKS[blockId] == NULL)
    {
        if (logInfoH()) 
            printf("The block with id %d is empty\n", blockId);
    }
    else
    {
        if (logInfoH()) 
            printf("Content of the block with id %d:\n%s", blockId, BLOCKS[blockId]);
    }
}

void freeAllBlocks()
{
    for (int i = 0; i < BLOCKS_COUNT; i++)
        free(BLOCKS[i]);
    free(BLOCKS);

    if (logInfoH()) 
        printf("Removed %d blocks\n", BLOCKS_COUNT);
}
