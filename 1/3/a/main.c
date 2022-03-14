#include "filestatslib.h"
#include "timemeaslib.h"
#include "loglib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>

#define MIN_PARAMETERS 1
const char* TEMP_FILE_PATH = "temp.tmp";

int isCommand(char* str)
{
    return (strcmp(str, "create_table") == 0 ||
            strcmp(str, "wc_files") == 0 ||
            strcmp(str, "remove_block") == 0 ||
            strcmp(str, "print_block") == 0);
}

int getFilePathsCount(int argc, char** argv, int i)
{
    int j = i;
    while (j < argc-1)
    {
        if (isCommand(argv[j+1]))
            break;
        j++;
    }

    return j - i + 1;
}

void moveToArray(char** arrFrom, char** arrTo, int iFrom, int length)
{
    for (int i = 0; i < length; i++)
        arrTo[i] = arrFrom[iFrom+i];
}

int main(int argc, char** argv)
{
    struct MeasuredTime mt = {0};
    struct MeasuredTime* mtPtr = &mt;
    clock_t clockStart, clockEnd;
    struct tms tmsStart, tmsEnd;

    int i = 0;
    while (i < argc-MIN_PARAMETERS)
    {
        if (i < argc-MIN_PARAMETERS && isCommand(argv[i+1]))
        {
            // Skip command 
        }
        else if (strcmp(argv[i], "create_table") == 0)
        {
            i++;
            int blocksCount = atoi(argv[i]);
            if (blocksCount >= 0)
            {
                clockStart = times(&tmsStart);
                createBlocks(blocksCount);
                clockEnd = times(&tmsEnd);
                saveTimes(&mtPtr->createFreeBlocks, tmsStart, tmsEnd, clockStart, clockEnd);
            }
        }
        else if (strcmp(argv[i], "wc_files") == 0)
        {
            i++;
            int filesCount = getFilePathsCount(argc, argv, i);
            char** filePaths = calloc(filesCount, sizeof(char *));
            moveToArray(argv, filePaths, i, filesCount);

            for (int j = 0; j < filesCount; j++)
            {
                if (logInfoH())
                    printf("Processing the file %s\n", filePaths[j]);

                clockStart = times(&tmsStart);
                usewc(filePaths[j], TEMP_FILE_PATH);
                clockEnd = times(&tmsEnd);
                saveTimes(&mtPtr->usewc, tmsStart, tmsEnd, clockStart, clockEnd);

                clockStart = times(&tmsStart);
                loadFileToMemory(TEMP_FILE_PATH);
                clockEnd = times(&tmsEnd);
                saveTimes(&mtPtr->loadFilesToMemory, tmsStart, tmsEnd, clockStart, clockEnd);
            }

            free(filePaths);
            i = i + filesCount - 1;
        }
        else if (strcmp(argv[i], "remove_block") == 0)
        {
            i++;
            int blockId = atoi(argv[i]);
            if (blockId >= 0)
            {
                clockStart = times(&tmsStart);
                freeBlock(blockId);
                clockEnd = times(&tmsEnd);
                saveTimes(&mtPtr->createFreeBlocks, tmsStart, tmsEnd, clockStart, clockEnd);
            }
        }
        else if (strcmp(argv[i], "print_block") == 0)
        {
            i++;
            int blockId = atoi(argv[i]);
            if (blockId >= 0)
                printBlock(blockId);
        }
        i++;
    }

    clockStart = times(&tmsStart);
    freeAllBlocks();
    clockEnd = times(&tmsEnd);
    saveTimes(&mtPtr->freeAllBlocks, tmsStart, tmsEnd, clockStart, clockEnd);

    printMeasuredTime(mt);

    return 0;
}
