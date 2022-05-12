#include "carrier.h"
#include "common.h"

int takePizza(int semSetId, struct Container* table)
{
    lockSem(semSetId, TABLE_EMPTY_SEM);
    lockSem(semSetId, TABLE_SEM);

    int pizzaType = table->pizzas[table->toTakeId];
    if (pizzaType == -1)
        exit(-1);

    table->pizzas[table->toTakeId] = -1;
    table->pizzasCnt--;
    table->toTakeId = (table->toTakeId + 1) % CONTAINER_CAPACITY;

    unlockSem(semSetId, TABLE_SEM);
    unlockSem(semSetId, TABLE_FULL_SEM);

    return pizzaType;
}

void mainLoop(int semSetId, struct Container* table)
{
    for (;;)
    {
        int pizzaType = takePizza(semSetId, table);
        printf("[$ CARR %d] (%s) Taking pizza: %d. Pizzas on table count: %d\n",
               getpid(), getCurrTime(), pizzaType, table->pizzasCnt);
        sleep(DELIVERY_TIME);

        printf("[$ CARR %d] (%s) Delivered pizza: %d\n", getpid(), getCurrTime(), pizzaType);
        sleep(RETURN_TIME);
    }
}

int main(int argc, char** argv)
{
    int semSetId = getSemSetId();
    int tableShmId = getTableShmId();
    struct Container* table = shmat(tableShmId, NULL, 0);

    mainLoop(semSetId, table);

    return 0;
}
