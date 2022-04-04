#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

#define SIGNAL SIGUSR1
#define MAX_TOPRINT_LEN 100

enum Action {ignore, handler, mask, pending};
enum ProcessType {parent, child};

enum ProcessType processType = parent;

enum Action strToAction(char* str)
{
    if (strcmp(str, "handler") == 0) return handler;
    if (strcmp(str, "mask") == 0) return mask;
    if (strcmp(str, "pending") == 0) return pending;
    return ignore;
}

enum ProcessType strToProcessType(char* str)
{
    if (strcmp(str, "child") == 0) return child;
    return parent;
}

void processPrint(char* toPrint)
{
    if (processType == parent)
        printf("[PARENT]");
    else
        printf("[CHILD]");
    printf(" %s\n", toPrint);
}

void executeChild(char* execPath, char* actionStr)
{
    char *args[] = {execPath, actionStr, "child", NULL};
    processPrint("Executing child");
    execvp(execPath, args);
}

void printAndRise()
{
    processPrint("Raising the signal");
    raise(SIGNAL);
}

void handlerFunc(int sigNum)
{
    char toPrint[MAX_TOPRINT_LEN];
    if (sigNum == SIGNAL)
        snprintf(toPrint, MAX_TOPRINT_LEN, "Received the signal");
    else
        snprintf(toPrint, MAX_TOPRINT_LEN, "Received signal %d", sigNum);
    processPrint(toPrint);
}

void checkIfPending()
{
    sigset_t sigSet;
    sigpending(&sigSet);
    if (sigismember(&sigSet, SIGNAL))
        processPrint("The signal is pending");
    else
        processPrint("The signal isn't pending");
}

void raiseBlocked()
{
    processPrint("Creating a blockage for the signal");
    struct sigaction act;
    act.sa_handler = handlerFunc;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGNAL);

    sigprocmask(SIG_BLOCK, &act.sa_mask, NULL);

    printAndRise();
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("[ERROR] Required parameters: action\n");
        return 0;
    }

    enum Action action = strToAction(argv[1]);
    if (argc > 2)
        processType = strToProcessType(argv[2]);

    if (processType == parent)
    {
        switch (action)
        {
            case ignore:
                signal(SIGNAL, SIG_IGN);
                printAndRise();
                executeChild(argv[0], argv[1]);
                break;
            case handler:
                signal(SIGNAL, handlerFunc);
                printAndRise();
                executeChild(argv[0], argv[1]);
                break;
            case mask:
            case pending:
                raiseBlocked();
                checkIfPending();            
                if (action == pending)
                    executeChild(argv[0], argv[1]);
                break;
        }
    }
    else
    {
        switch (action)
        {
            case ignore:
            case handler:
                printAndRise();
                break;
            case mask:
            case pending:
                checkIfPending();            
                break;
        }
    }

    return 0;
}
