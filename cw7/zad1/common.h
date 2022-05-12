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
extern const size_t CONTAINER_SIZE;

union semun 
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

void checkError(int errorCode, char* toPrint);
char* getCurrTime();
void lockSem(int semId, int semNum);
void unlockSem(int semId, int semNum);
int getSemSetId();
int getContainerId(char* pathName, char proj);
int getOvenShmId();
int getTableShmId();

#endif
