#include "timemeaslib.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <time.h>

#define A 0
#define B 1
#define MAX_INT_LEN 10 

double f(double x)
{
    return 4/(x*x + 1);
}

char* getFileName(int processId)
{
    int fileNameLen = MAX_INT_LEN + 5;
    char* fileName = calloc(fileNameLen, sizeof(char));
    
    if (snprintf(fileName, fileNameLen, "w%d.txt", processId) < 0)
    {
        printf("[ERROR] Encoding error occurred for a file name\n");
        exit(1);
    }

    return fileName;
}

FILE* openFile(char* fileName, char* mode)
{
    FILE* fPtr = fopen(fileName, mode);
    if (fPtr == NULL)
    {
        printf("[ERROR] Couldn't open the file %s\n", fileName);
        exit(2);
    }

    return fPtr;
}

void saveResultToFile(double result, int processId)
{
    char* fileName = getFileName(processId);
    FILE* fPtr = openFile(fileName, "w+");
    if (fprintf(fPtr, "%lf", result) < 0)
    {
        printf("[ERROR] Couldn't write to the file %s\n", fileName);
        exit(3);
    }
    free(fileName);
    fclose(fPtr);
}

void childProcessHandler(int processId, int rectanglesPerInterval, 
                         int leftoverRectangles, double intervalWidth)
{
    int i = processId - 1;
    int start = i * rectanglesPerInterval;
    int end = 0;
    if (i < leftoverRectangles)
    {
        start += i;
        end = 1;
    }
    else
        start += leftoverRectangles;
    end += start + rectanglesPerInterval;

    double result = 0;
    for (int j = start; j < end; ++j)
        result += f(j*intervalWidth) * intervalWidth;
    saveResultToFile(result, processId);
}

double sumResultsFromFiles(int maxProcessId)
{
    double result = 0, tmp;
    for (int processId = 1; processId <= maxProcessId; ++processId)
    {
        char* fileName = getFileName(processId);
        FILE* fPtr = openFile(fileName, "r");
        if (fscanf(fPtr, "%lf", &tmp) < 1)
        {
            printf("[ERROR] Couldn't read from the file %s\n", fileName);
            exit(4);
        }
        free(fileName);
        fclose(fPtr);
        result += tmp;
    }

    return result;
}

int main(int argc, char** argv)
{
    struct MeasuredTime mt = {0};
    struct MeasuredTime* mtPtr = &mt;
    clock_t clockStartTotal, clockEndTotal;
    struct tms tmsStartTotal, tmsEndTotal;
    clockStartTotal = times(&tmsStartTotal);

    if (argc < 3)
    {
        printf("[ERROR] Required parameters: rectangleWidth, n\n");
        return -1;
    }

    double rectangleWidth;
    sscanf(argv[1], "%lf", &rectangleWidth);
    int n = atoi(argv[2]);

    double span = (double)B - (double)A;
    int rectanglesCnt = (int)(span / rectangleWidth) + 
                        (int)(rectangleWidth > span);
    int rectanglesPerInterval = rectanglesCnt / n;
    int leftoverRectangles = rectanglesCnt % n;
    double intervalWidth = span / (double)rectanglesCnt;
    int status;

    for (int processId = 1; processId <= n; ++processId)
    {
        if (fork() == 0) 
        {
            childProcessHandler(processId, rectanglesPerInterval, 
                                leftoverRectangles, intervalWidth);
            return 0;
        }
    }

    while (wait(&status) > 0);

    printf("Integral of f(x) = 4/(x*x + 1) for A = %lf, B = %lf is equal to %lf\n", 
           (double)A, (double)B, sumResultsFromFiles(n));

    clockEndTotal = times(&tmsEndTotal);
    saveTimes(&mtPtr->total, tmsStartTotal, tmsEndTotal, clockStartTotal, clockEndTotal);

    printMeasuredTime(mt);

    return 0;
}
