#ifndef CARRIER
#define CARRIER

#include "common.h"

int takePizza(int semSetId, struct Container* table);
void mainLoop(int semSetId, struct Container* table);
int main(int argc, char** argv);

#endif
