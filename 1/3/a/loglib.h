#ifndef LOGLIB
#define LOGLIB

#include <stdio.h>

int logInfoH();
int logSuccessH();
int logErrorH();
void logInfo(char* message);
void logSuccess(char* message);
void logError(char* message);

#endif
