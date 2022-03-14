#include <stdio.h>

#define LOGLIB_INFO_LOGGING 0
#define LOGLIB_SUCCESS_LOGGING 0
#define LOGLIB_ERROR_LOGGING 1

int logInfoH()
{
    if (LOGLIB_INFO_LOGGING)
        printf("[INFO] ");

    return LOGLIB_INFO_LOGGING;
}

int logSuccessH()
{
    if (LOGLIB_SUCCESS_LOGGING)
        printf("[SUCCESS] ");

    return LOGLIB_SUCCESS_LOGGING;
}

int logErrorH()
{
    if (LOGLIB_ERROR_LOGGING)
        printf("[ERROR] ");

    return LOGLIB_ERROR_LOGGING;
}

void logInfo(char* message)
{
    if (logInfoH())
        printf(message);
}

void logError(char* message)
{
    if (logErrorH())
        printf(message);
}

void logSuccess(char* message)
{
    if (logSuccessH())
        printf(message);
}
