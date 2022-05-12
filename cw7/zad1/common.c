#include "common.h"

const size_t CONTAINER_SIZE = sizeof(struct Container);

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
    checkError(key, "Failed to access the semaphore set key");
    int semSetId = semget(key, SEMS_CNT, 0);
    checkError(semSetId, "Failed to access the semaphore set id");
    return semSetId;
}

int getContainerId(char* pathName, char proj)
{
    key_t key = ftok(pathName, proj);
    checkError(key, "Failed to access a key");
    int shmId = shmget(key, CONTAINER_SIZE, 0);
    checkError(shmId, "Failed to access a shared memory id");
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
