#include "timemeaslib.h"
#include <stdio.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>

double timeElapsedInSeconds(clock_t clockStart, clock_t clockEnd) 
{
    clock_t clockDiff = clockEnd - clockStart;
    return (double)clockDiff / sysconf(_SC_CLK_TCK);
}

void saveTimes(struct TimeType* ttPtr, struct tms tmsStart, struct tms tmsEnd, clock_t clockStart, clock_t clockEnd)
{
    ttPtr->real += timeElapsedInSeconds(clockStart, clockEnd);
    ttPtr->user += timeElapsedInSeconds(tmsStart.tms_utime, tmsEnd.tms_utime);
    ttPtr->sys += timeElapsedInSeconds(tmsStart.tms_stime, tmsEnd.tms_stime);
}

void printTimeType(char* taskName, struct TimeType tt)
{
    printf("|%15s |%15f |%15f |%15f |\n", taskName, tt.real, tt.user, tt.sys);
}

void printMeasuredTime(struct MeasuredTime mt)
{
    int tableWidth = 69;
    printHorizontalLine('-', tableWidth);
    printf("|%15s |%15s |%15s |%15s |\n", "Task", "real[s]", "user[s]", "sys[s]");
    printHorizontalLine('-', tableWidth);
    printTimeType("Total", mt.total);
    printHorizontalLine('-', tableWidth);
    printf("\n");
}

void printHorizontalLine(char repr, int width)
{
    for (int i = 0; i < width; i++)
        printf("%c", repr);
    printf("\n");
}
