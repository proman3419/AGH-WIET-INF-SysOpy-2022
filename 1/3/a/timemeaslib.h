#ifndef TIMEMEASLIB
#define TIMEMEASLIB

#include <sys/times.h>
#include <time.h>

struct TimeType
{
    double real;
    double user;
    double sys;
};

struct MeasuredTime
{
    struct TimeType usewc;
    struct TimeType loadFilesToMemory;
    struct TimeType freeAllBlocks;
    struct TimeType createFreeBlocks;
};

double timeElapsedInSeconds(clock_t clockStart, clock_t clockEnd);
void saveTimes(struct TimeType* tt, struct tms tmsStart, struct tms tmsEnd, clock_t clockStart, clock_t clockEnd);
void printTimeType(struct TimeType tt);
void printMeasuredTime(struct MeasuredTime mt);

#endif
