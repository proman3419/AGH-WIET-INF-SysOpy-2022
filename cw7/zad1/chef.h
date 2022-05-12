#ifndef CHEF
#define CHEF

#include "common.h"

void putInOven(int semSetId, struct Container* oven, int pizzaType);
int takeOutFromOven(int semSetId, struct Container* oven);
void putOnTable(int semSetId, struct Container* table, int pizzaType);
void mainLoop(int semSetId, struct Container* oven, struct Container* table);
int main(int argc, char** argv);

#endif
