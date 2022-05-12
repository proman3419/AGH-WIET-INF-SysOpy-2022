#ifndef COMMON
#define COMMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/time.h>

#define OVEN_PATH "oven"
#define TABLE_PATH "table"

#define OVEN_PROJ 'O'
#define TABLE_PROJ 'T'
#define SEMSET_PROJ 'S'

#define OVEN_SEM 0
#define OVEN_FULL_SEM 1
#define TABLE_SEM 2
#define TABLE_FULL_SEM 3
#define TABLE_EMPTY_SEM 4

#define PREPARATION_TIME 2
#define BAKING_TIME 4
#define DELIVERY_TIME 4
#define RETURN_TIME 4

#define CONTAINER_CAPACITY 3
#define PERMISSIONS 0666
#define SEMS_CNT 5

struct Container
{
    int pizzas[CONTAINER_CAPACITY];
    int pizzasCnt;
    int toPutId;
    int toTakeId;
};
const size_t CONTAINER_SIZE = sizeof(struct Container);

union semun 
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

void checkError(int errorCode, char* toPrint)
{
    if (errorCode == -1)
    {
        printf("%s, errno: %d\n", toPrint, errno);
        exit(-1);
    }
}

char* getCurrTime()
{
    struct timeval currTime;
    gettimeofday(&currTime, NULL);

    const int BUFFER_SIZE = 64;
    char buffer[BUFFER_SIZE];
    char* bufferPtr = &buffer[0];
    strftime(bufferPtr, BUFFER_SIZE, "%H:%M:%S", localtime(&currTime.tv_sec));

    char* currTimeStr = calloc(BUFFER_SIZE, sizeof(char));
    int msec = currTime.tv_usec / 1000;
    sprintf(currTimeStr, "%s:%03d", bufferPtr, msec);

    return currTimeStr;
}

void lockSem(int semId, int semNum)
{
    struct sembuf semBuf = {.sem_num = semNum, .sem_op = -1};
    semop(semId, &semBuf, 1);
}

void unlockSem(int semId, int semNum)
{
    struct sembuf semBuf = {.sem_num = semNum, .sem_op = 1};
    semop(semId, &semBuf, 1);
}

int getSemSetId()
{
    key_t key = ftok(getenv("HOME"), SEMSET_PROJ);
    int semSetId = semget(key, SEMS_CNT, 0);
    return semSetId;
}

int getContainerId(char* pathName, char proj)
{
    key_t key = ftok(pathName, proj);
    int shmId = shmget(key, CONTAINER_SIZE, 0);
    return shmId;
}

int getOvenShmId()
{
    return getContainerId(OVEN_PATH, OVEN_PROJ);
}

int getTableShmId()
{
    return getContainerId(TABLE_PATH, TABLE_PROJ);
}

#endif
