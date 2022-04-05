#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

#define SIGNAL SIGUSR1
#define MAX_TOPRINT_LEN 100

enum Action {ignore, handler, mask, pending};

int parentPID;

enum Action strToAction(char* str)
{
    if (strcmp(str, "ignore") == 0) return ignore;
    if (strcmp(str, "handler") == 0) return handler;
    if (strcmp(str, "mask") == 0) return mask;
    if (strcmp(str, "pending") == 0) return pending;
    return ignore;
}

void processPrint(char* toPrint)
{
    if (getpid() == parentPID)
        printf("[PARENT]");
    else
        printf("[CHILD]");
    printf(" %s\n", toPrint);
}

void printAndRaise()
{
    processPrint("Raising the signal");
    raise(SIGNAL);
}

void raiseAndFork()
{
    printAndRaise();
    processPrint("Forking");
    if (fork() == 0)
        printAndRaise();
    else
        wait(NULL);
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

    printAndRaise();
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("[ERROR] Required parameters: action\n");
        return -1;
    }

    enum Action action = strToAction(argv[1]);
    parentPID = (int)getpid();

    switch (action)
    {
        case ignore:
            signal(SIGNAL, SIG_IGN);
            raiseAndFork();
            break;
        case handler:
            signal(SIGNAL, handlerFunc);
            raiseAndFork();
            break;
        case mask:
        case pending:
            raiseBlocked();
            checkIfPending();
            processPrint("Forking");
            if (fork() == 0)
            {
                if (action == mask)
                    printAndRaise();
                checkIfPending();
            }
            else
                wait(NULL);
            break;
    }

    return 0;
}
