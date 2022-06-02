#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#define REINDEER_CNT 9
#define ELVES_CNT 10
#define ELVES_WAITING_CNT 3
#define MAX_DELIVERIES 3

struct ThreadContainer
{
    int id;
    pthread_t thread;
};

int deliveries = 0;

int elvesWaiting = 0;
int elvesQueue[ELVES_WAITING_CNT];

int reindeerReturned = 0;
int reindeerWaiting = 0;

pthread_t santaThread;
struct ThreadContainer elves[ELVES_CNT];
struct ThreadContainer reindeer[REINDEER_CNT];

pthread_mutex_t santaMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santaCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elfMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elfWaitMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elfWaitCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t reindeerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeerWaitMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeerWaitCond = PTHREAD_COND_INITIALIZER;

int getRandInt(int min, int max) // both inclusive
{
    return rand() % (max+1 - min) + min;
}

void resetElvesQueue()
{
    for (int i = 0; i < ELVES_WAITING_CNT; ++i)
        elvesQueue[i] = -1;
}

void cancelAll(struct ThreadContainer* threadContainers, int threadContainersCnt)
{
    for (int i = 0; i < threadContainersCnt; ++i)
        pthread_cancel(threadContainers[i].thread);
}

void exitHandler()
{
    cancelAll(elves, ELVES_CNT);
    cancelAll(reindeer, REINDEER_CNT);

    pthread_mutex_destroy(&santaMutex);
    pthread_mutex_destroy(&elfMutex);
    pthread_mutex_destroy(&elfWaitMutex);
    pthread_mutex_destroy(&reindeerMutex);
    pthread_mutex_destroy(&reindeerWaitMutex);

    pthread_cond_destroy(&santaCond);
    pthread_cond_destroy(&elfWaitCond);
    pthread_cond_destroy(&reindeerWaitCond);

    printf("[# SANTA] Merry Christmas\n");
}

void signalHandler()
{
    exit(0);
}

void initThreadContainers(struct ThreadContainer* threadContainers, 
                          int threadContainersCnt, void* threadFunc)
{
    for (int i = 0; i < threadContainersCnt; ++i)
    {
        threadContainers[i].id = i;
        pthread_create(&threadContainers[i].thread, NULL, 
                       threadFunc, &threadContainers[i].id);
    }
}

void joinAll(struct ThreadContainer* threadContainers, int threadContainersCnt)
{
    for (int i = 0; i < threadContainersCnt; ++i)
        pthread_join(threadContainers[i].thread, NULL);
}

void init()
{
    if (atexit(exitHandler) != 0)
    {
        printf("[ERROR] Couldn't set atexit function\n");
        exit(-1);
    }

    struct sigaction sigAct;
    sigemptyset(&sigAct.sa_mask);
    sigAct.sa_handler = signalHandler;

    if (sigaction(SIGINT, &sigAct, NULL) == -1)
    {
        printf("[ERROR] Couldn't set signal handler\n");
        exit(-1);
    }

    srand(time(NULL));
    resetElvesQueue();
}

void handleElves()
{
    pthread_mutex_lock(&elfMutex);
    if (elvesWaiting == ELVES_WAITING_CNT)
    {
        printf("[# SANTA] Solving elves' ");
        for (int i = 0; i < ELVES_WAITING_CNT; ++i)
            printf("%d ", elvesQueue[i]);
        printf("problems\n");

        sleep(getRandInt(1, 2));

        pthread_mutex_lock(&elfWaitMutex);
        resetElvesQueue();
        elvesWaiting = 0;
        pthread_cond_broadcast(&elfWaitCond);
        pthread_mutex_unlock(&elfWaitMutex);
    }
    pthread_mutex_unlock(&elfMutex);
}

void handleReindeer()
{
    pthread_mutex_lock(&reindeerMutex);
    if (reindeerReturned == REINDEER_CNT) 
    {
        ++deliveries;
        printf("[# SANTA] Delivering toys %d. time !!!!!\n", deliveries);
        sleep(getRandInt(2, 4));
        reindeerReturned = 0;

        pthread_mutex_lock(&reindeerWaitMutex);
        reindeerWaiting = 0;
        pthread_cond_broadcast(&reindeerWaitCond);
        pthread_mutex_unlock(&reindeerWaitMutex);
    }
    pthread_mutex_unlock(&reindeerMutex);
}

void* santaFunc(void* arg)
{
    for (;;)
    {
        pthread_mutex_lock(&santaMutex);
        while (elvesWaiting < ELVES_WAITING_CNT && 
               reindeerReturned < REINDEER_CNT)
            pthread_cond_wait(&santaCond, &santaMutex);
        pthread_mutex_unlock(&santaMutex);

        printf("[# SANTA] Waking up\n");
        handleReindeer();
        if (deliveries == MAX_DELIVERIES)
            exit(0);
        handleElves();
        printf("[# SANTA] Falling asleep\n");
    }
}

void* elfFunc(void* arg)
{
    int id = *((int*)arg);
    for (;;)
    {
        sleep(getRandInt(2, 5));

        pthread_mutex_lock(&elfWaitMutex);
        while (elvesWaiting == ELVES_WAITING_CNT)
        {
            printf("[$ ELF] Waiting for elves to come back, %d\n", id);
            pthread_cond_wait(&elfWaitCond, &elfWaitMutex);
        }
        pthread_mutex_unlock(&elfWaitMutex);

        pthread_mutex_lock(&elfMutex);
        if (elvesWaiting < ELVES_WAITING_CNT)
        {
            elvesQueue[elvesWaiting++] = id;
            printf("[$ ELF] %d elves are waiting for santa, %d\n", 
                   elvesWaiting, id);

            if (elvesWaiting == ELVES_WAITING_CNT)
            {
                printf("[$ ELF] Waking santa up, %d\n", id);
                pthread_mutex_lock(&santaMutex);
                pthread_cond_broadcast(&santaCond);
                pthread_mutex_unlock(&santaMutex);
            }
        }
        pthread_mutex_unlock(&elfMutex);
    }
}

void* reindeerFunc(void* arg)
{
    int id = *((int*)arg);
    for (;;) 
    {
        pthread_mutex_lock(&reindeerWaitMutex);
        while (reindeerWaiting == 1)
            pthread_cond_wait(&reindeerWaitCond, &reindeerWaitMutex);
        pthread_mutex_unlock(&reindeerWaitMutex);

        sleep(getRandInt(5, 10));

        pthread_mutex_lock(&reindeerMutex);
        ++reindeerReturned;
        printf("[@ REINDEER] %d reindeer, %d\n", reindeerReturned, id);
        reindeerWaiting = 1;

        if (reindeerReturned == REINDEER_CNT)
        {
            printf("[@ REINDEER] Waking santa up, %d\n", id);
            pthread_mutex_lock(&santaMutex);
            pthread_cond_broadcast(&santaCond);
            pthread_mutex_unlock(&santaMutex);
        }
        pthread_mutex_unlock(&reindeerMutex);
    }
}

int main()
{
    init();

    pthread_create(&santaThread, NULL, &santaFunc, NULL);
    initThreadContainers(elves, ELVES_CNT, elfFunc);
    initThreadContainers(reindeer, REINDEER_CNT, reindeerFunc);

    pthread_join(santaThread, NULL);
    joinAll(elves, ELVES_CNT);
    joinAll(reindeer, REINDEER_CNT);

    return 0;
}
