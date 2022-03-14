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

double timeElapsedInSeconds(clock_t clockStart, clock_t clockEnd);
void saveTimes(struct TimeType* tt, struct tms tmsStart, struct tms tmsEnd, clock_t clockStart, clock_t clockEnd);
void printTimeType(struct TimeType tt);

#endif
