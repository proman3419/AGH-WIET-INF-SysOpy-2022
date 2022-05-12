#include "chef.h"
#include "common.h"

void putInOven(int semSetId, struct Container* oven, int pizzaType)
{
    lockSem(semSetId, OVEN_FULL_SEM);
    lockSem(semSetId, OVEN_SEM);

    oven->pizzas[oven->toPutId] = pizzaType;
    oven->toPutId = (oven->toPutId + 1) % CONTAINER_CAPACITY;
    oven->pizzasCnt++;

    unlockSem(semSetId, OVEN_SEM);
}

int takeOutFromOven(int semSetId, struct Container* oven)
{
    lockSem(semSetId, OVEN_SEM);

    int pizzaType = oven->pizzas[oven->toTakeId];
    if (pizzaType == -1)
        exit(-1);

    oven->pizzas[oven->toTakeId] = -1;
    oven->toTakeId = (oven->toTakeId + 1) % CONTAINER_CAPACITY;
    oven->pizzasCnt--;

    unlockSem(semSetId, OVEN_SEM);
    unlockSem(semSetId, OVEN_FULL_SEM);

    return pizzaType;
}

void putOnTable(int semSetId, struct Container* table, int pizzaType)
{
    lockSem(semSetId, TABLE_FULL_SEM);
    lockSem(semSetId, TABLE_SEM);

    table->pizzas[table->toPutId] = pizzaType;
    table->toPutId = (table->toPutId + 1) % CONTAINER_CAPACITY;
    table->pizzasCnt++;

    unlockSem(semSetId, TABLE_SEM);
    unlockSem(semSetId, TABLE_EMPTY_SEM);
}

void mainLoop(int semSetId, struct Container* oven, struct Container* table)
{
    for (;;)
    {
        int pizzaType = rand() % 10;
        printf("[# CHEF %d] (%s) Preparing pizza: %d\n",
               getpid(), getCurrTime(), pizzaType);
        sleep(PREPARATION_TIME);

        putInOven(semSetId, oven, pizzaType);
        printf("[# CHEF %d] (%s) Added pizza: %d. Pizzas in oven count: %d\n",
               getpid(), getCurrTime(), pizzaType, oven->pizzasCnt);
        sleep(BAKING_TIME);

        pizzaType = takeOutFromOven(semSetId, oven);
        putOnTable(semSetId, table, pizzaType);
        printf("[# CHEF %d] (%s) Taking out pizza: %d. Pizzas in oven count: %d. Pizzas on table count: %d\n", 
               getpid(), getCurrTime(), pizzaType, 
               oven->pizzasCnt, table->pizzasCnt);
    }
}

int main(int argc, char** argv)
{
    int semSetId = getSemSetId();
    int ovenShmId = getOvenShmId();
    int tableShmId = getTableShmId();

    struct Container* oven = shmat(ovenShmId, NULL, 0);
    struct Container* table = shmat(tableShmId, NULL, 0);

    srand(getpid());

    mainLoop(semSetId, oven, table);

    return 0;
}
