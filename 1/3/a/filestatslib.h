#ifndef FILESTATSLIB
#define FILESTATSLIB

#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <time.h>

struct TimeType
{
    double real;
    double user;
    double sys;
};

struct ExecutionTime
{
    struct TimeType usewc;
    struct TimeType loadFilesToMemory;
    struct TimeType freeAllBlocks;
    struct TimeType createFreeBlocks;
};

void createBlocks(int blocksCount);
void gatherStats(char** filePaths, int filesCount, char* tempFilePath, struct ExecutionTime* et);
void usewc(char* filePath, char* tempFilePath);
int loadFileToMemory(char* filePath);
void freeBlock(int blockId);
long getFileSize(FILE* filePointer);
void printBlock(int blockId);
void freeAllBlocks();

double timeElapsedInSeconds(clock_t clockStart, clock_t clockEnd);
void saveTimes(struct TimeType* tt, struct tms tmsStart, struct tms tmsEnd, clock_t clockStart, clock_t clockEnd);
void printTimeType(struct TimeType tt);
void printExecutionTime(struct ExecutionTime et);

#endif
