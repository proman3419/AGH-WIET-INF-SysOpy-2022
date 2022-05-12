#include "common.h"

int ovenShmId, tableShmId, semSetId;

int init(char* pathName, char proj)
{
    key_t key = ftok(pathName, proj);
    checkError(key, "Failed to create a key");

    int shmId = shmget(key, CONTAINER_SIZE, IPC_CREAT | PERMISSIONS);
    checkError(shmId, "Failed to create a shared memory segment");

    struct Container* container = shmat(shmId, NULL, 0);

    for (int i = 0; i < CONTAINER_CAPACITY; ++i)
        container->pizzas[i] = -1;
    container->pizzasCnt = 0;
    container->toPutId = 0;
    container->toTakeId = 0;

    return shmId;
}

void createSemSet()
{
    key_t key = ftok(getenv("HOME"), SEMSET_PROJ);
    checkError(key, "Failed to create a key");
    
    semSetId = semget(key, SEMS_CNT, IPC_CREAT | PERMISSIONS);
    checkError(semSetId, "Couldn't create a semaphore set");

    union semun arg;
    char errorMsg[] = "Failed to set semaphore value";

    arg.val = 1;
    checkError(semctl(semSetId, OVEN_SEM, SETVAL, arg), errorMsg);
    checkError(semctl(semSetId, TABLE_SEM, SETVAL, arg), errorMsg);

    arg.val = CONTAINER_CAPACITY;
    checkError(semctl(semSetId, OVEN_FULL_SEM, SETVAL, arg), errorMsg);
    checkError(semctl(semSetId, TABLE_FULL_SEM, SETVAL, arg), errorMsg);

    arg.val = 0;
    checkError(semctl(semSetId, TABLE_EMPTY_SEM, SETVAL, arg), errorMsg);

    printf("Created semaphore set, semSetId: %d\n", semSetId);
}

void sigintHandler(int signum)
{
    semctl(semSetId, 0, IPC_RMID, NULL);
    shmctl(ovenShmId, IPC_RMID, NULL);
    shmctl(tableShmId, IPC_RMID, NULL);
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        printf("[ERROR] Required parameters: chefsCnt, carriersCnt\n");
        return -1;
    }

    int chefsCnt = atoi(argv[1]);
    int carriersCnt = atoi(argv[2]);

    ovenShmId = init(OVEN_PATH, OVEN_PROJ);
    tableShmId = init(TABLE_PATH, TABLE_PROJ);
    createSemSet();

    signal(SIGINT, sigintHandler);

    for (int i = 0; i < chefsCnt; ++i)
        if (fork() == 0)
            execl("./output_chef", "./output_chef", NULL);

    for (int i = 0; i < carriersCnt; ++i)
        if (fork() == 0)
            execl("./output_carrier", "./output_carrier", NULL);

    while (wait(NULL) > 0);
    
    return 0;
}
