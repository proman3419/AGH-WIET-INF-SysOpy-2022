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
    struct TimeType createBlocks;
    struct TimeType removeBlocks;
    struct TimeType total;
};

double timeElapsedInSeconds(clock_t clockStart, clock_t clockEnd);
void saveTimes(struct TimeType* tt, struct tms tmsStart, struct tms tmsEnd, clock_t clockStart, clock_t clockEnd);
void printTimeType(char* taskName, struct TimeType tt);
void printMeasuredTime(struct MeasuredTime mt);
void printHorizontalLine(char repr, int width);

#endif
