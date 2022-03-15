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

void printTimeType(char* taskName, struct TimeType tt)
{
    printf("|%25s |%15f |%15f |%15f |\n", taskName, tt.real, tt.user, tt.sys);
}

void printMeasuredTime(struct MeasuredTime mt)
{
    int tableWidth = 79;
    printHorizontalLine('-', tableWidth);
    printf("|%25s |%15s |%15s |%15s |\n", "Task", "real [s]", "user [s]", "sys [s]");
    printHorizontalLine('-', tableWidth);
    printTimeType("Using the wc program", mt.usewc);
    printTimeType("Loading to memory blocks", mt.loadFilesToMemory);
    printTimeType("Creating blocks", mt.createBlocks);
    printTimeType("Removing blocks", mt.removeBlocks);
    printHorizontalLine('-', tableWidth);
    printf("\n");
}

void printHorizontalLine(char repr, int width)
{
    for (int i = 0; i < width; i++)
        printf("%c", repr);
    printf("\n");
}
