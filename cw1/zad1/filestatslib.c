#include "filestatslib.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <time.h>

#define MAX_COMMAND_LENGTH 1024

char** BLOCKS;
int BLOCKS_COUNT;

void createBlocksPlaceholders(int blocksCount)
{
    if (BLOCKS != NULL)
        removeAllBlocks();
    BLOCKS = calloc(blocksCount, sizeof(char*));
    BLOCKS_COUNT = blocksCount;
    printf("[INFO] Created %d blocks placeholders\n", BLOCKS_COUNT);
}

void createBlocks(int blocksCount, int blockByteSize)
{
    createBlocksPlaceholders(blocksCount);
    for (int i = 0; i < BLOCKS_COUNT; i++)
        BLOCKS[i] = calloc(BLOCKS_COUNT, blockByteSize);
    printf("[INFO] Created %d blocks of size %dB\n", BLOCKS_COUNT, blockByteSize);
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
        printf("[ERROR] Encoding error occurred for the command\n");
    else if (commandLength > MAX_COMMAND_LENGTH)
        printf("[ERROR] The command is too long\n");
    else
    {
        status = system(command);

        if (status == -1)
            printf("[ERROR] Failed to execute the command '%s'\n", command);
    }

    free(command);
}

int loadFileToMemory(const char* filePath)
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

    result[fileSize-1] = '\0';

    for (int i = 0; i < BLOCKS_COUNT; i++)
    {
        if (BLOCKS[i] == NULL)
        {
            BLOCKS[i] = result;
            printf("[SUCCESS] Content of %s loaded at the block with id %d\n", filePath, i);
            return i;
        }
    }

    printf("[ERROR] No free space in the BLOCKS array\n");

    return -1;
}

void removeBlock(int blockId)
{
    if (BLOCKS == NULL)
        printf("[ERROR] The BLOCKS array is NULL, call createBlocksPlaceholders first\n");
    else if (BLOCKS[blockId] != NULL)
    {
        free(BLOCKS[blockId]);
        BLOCKS[blockId] = NULL;
        printf("[INFO] Removed the block with id %d\n", blockId);
    }
    else
        printf("[INFO] The block with id %d was empty, didn't remove anything\n", blockId);
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
    if (BLOCKS == NULL)
        printf("[ERROR] The BLOCKS array is NULL, call createBlocksPlaceholders first\n");
    else if (blockId < 0 || BLOCKS_COUNT <= blockId)
        printf("[ERROR] Block id should be from range [0, %d], was %d\n", BLOCKS_COUNT, blockId);
    else if (BLOCKS[blockId] == NULL)
        printf("[INFO] The block with id %d is empty\n", blockId);
    else
        printf("[INFO] Content of the block with id %d:\n%s", blockId, BLOCKS[blockId]);
}

void removeAllBlocks()
{
    if (BLOCKS != NULL)
    {
        for (int i = 0; i < BLOCKS_COUNT; i++)
        {
            free(BLOCKS[i]);
            BLOCKS[i] = NULL;
        }
        free(BLOCKS);
        BLOCKS = NULL;

        printf("[INFO] Removed %d blocks\n", BLOCKS_COUNT);
    }
    else
        printf("[INFO] The BLOCKS array was empty, didn't remove anything\n");
}
