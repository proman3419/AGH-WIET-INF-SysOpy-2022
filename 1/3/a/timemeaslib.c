#include "timemeaslib.h"
#include <stdio.h>
#include <sys/times.h>
#include <time.h>

double timeElapsedInSeconds(clock_t clockStart, clock_t clockEnd) 
{
    clock_t clockDiff = clockEnd - clockStart;
    return (double)clockDiff / CLOCKS_PER_SEC;
}

void saveTimes(struct TimeType* ttPtr, struct tms tmsStart, struct tms tmsEnd, clock_t clockStart, clock_t clockEnd)
{
    ttPtr->real += timeElapsedInSeconds(clockStart, clockEnd);
    ttPtr->user += timeElapsedInSeconds(tmsStart.tms_utime, tmsEnd.tms_utime);
    ttPtr->sys += timeElapsedInSeconds(tmsStart.tms_stime, tmsEnd.tms_stime);
}

void printTimeType(struct TimeType tt)
{
    printf("real %fs | ", tt.real);
    printf("user %fs | ", tt.user);
    printf("sys  %fs\n", tt.sys);
}

void printMeasuredTime(struct MeasuredTime mt)
{
    printf("wc: ");
    printTimeType(mt.usewc);
    printf("Loading to memory blocks: ");
    printTimeType(mt.loadFilesToMemory);
    printf("Removing all blocks: ");
    printTimeType(mt.freeAllBlocks);
    printf("Adding/removing blocks: ");
    printTimeType(mt.createFreeBlocks);
}
