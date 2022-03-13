#include "filestatslib.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_COMMAND_LENGTH 1024

char** BLOCKS;
int BLOCKS_COUNT;

void createBlocks(int blocksCount)
{
    BLOCKS = calloc(blocksCount, sizeof(char*));
    BLOCKS_COUNT = blocksCount;
    printf("[INFO] Created %d blocks\n", BLOCKS_COUNT);
}

void gatherStats(char** filePaths, int filesCount, char* tempFilePath)
{
    char* command = calloc(MAX_COMMAND_LENGTH, sizeof(char));
    int status, commandLength;

    for (int i = 0; i < filesCount; i++)
    {
        printf("[INFO] Processing the file %s\n", filePaths[i]);

        commandLength = snprintf(command, MAX_COMMAND_LENGTH,
                                     "wc --lines --words --chars %s > %s", 
                                     filePaths[i], tempFilePath);

        if (commandLength < 0)
            printf("[ERROR] Encoding error occurred for the command\n");
        else if (commandLength > MAX_COMMAND_LENGTH)
            printf("[ERROR] The command is too long\n");
        else
        {
            status = system(command);

            if (status == -1)
                printf("[ERROR] Failed to execute the command '%s'\n", command);
        }

        loadFileToMemory(tempFilePath);
    }

    free(command);
}

int loadFileToMemory(char* filePath)
{
    FILE* filePointer = fopen(filePath, "r");
    if (filePointer == NULL) 
    {
        printf("[ERROR] Couldn't open the file %s\n", filePath);
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
            printf("[SUCCESS] Content of %s loaded at the block with id %d\n", 
                   filePath, i);
            return i;
        }
    }

    printf("[ERROR] No free space in the BLOCKS array\n");

    return -1;
}

void freeBlock(int blockId)
{
    free(BLOCKS[blockId]);
    BLOCKS[blockId] = NULL;
    printf("[INFO] Removed the block with id %d\n", blockId);
}

long getFileSize(FILE* filePointer)
{
    if (fseek(filePointer, 0, SEEK_END) == 0)
        return ftell(filePointer);

    printf("[ERROR] Couldn't get the file size for %p\n", filePointer);

    return -1;
}

void printBlock(int blockId)
{
    if (blockId < 0 || BLOCKS_COUNT <= blockId)
        printf("[ERROR] Block id should be from range [0, %d], was %d\n", BLOCKS_COUNT, blockId);
    else if (BLOCKS[blockId] == NULL)
        printf("[INFO] The block with id %d is empty\n", blockId);
    else
        printf("[INFO] Content of the block with id %d:\n%s", blockId, BLOCKS[blockId]);
}

void freeAll()
{
    for (int i = 0; i < BLOCKS_COUNT; i++)
        free(BLOCKS[i]);
    free(BLOCKS);

    printf("[INFO] Freed the dynamically allocated memory\n");
}
