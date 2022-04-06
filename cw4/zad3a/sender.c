#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

enum SendingMode {KILL, SIGQUEUE, SIGRT};

int SEND_SIGNAL = SIGUSR1;
int FINISH_SIGNAL = SIGUSR2;
int signalsReceivedCnt = 0;
int signalsSentCnt;

enum SendingMode strToSendingMode(char* str)
{
    if (strcmp(str, "kill") == 0) return KILL;
    if (strcmp(str, "sigqueue") == 0) return SIGQUEUE;
    return SIGRT;
}

void setSigAction(struct sigaction act, void (*handlerFunc)(int, siginfo_t*, void*), int sigNo, int flag)
{
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handlerFunc;
    act.sa_flags = flag;
    sigaction(sigNo, &act, NULL);
}

void sendSignalHandler(int signo, siginfo_t* info, void* context)
{
    if (signo != SEND_SIGNAL)
        return;

    ++signalsReceivedCnt;
    printf("Received a SEND_SIGNAL from catcher\n");
}

void finishSignalHandler(int signo, siginfo_t* info, void* context)
{
    if (signo != FINISH_SIGNAL)
        return;

    printf("Received a FINISH_SIGNAL from catcher\n");
}

void sendSignals(int catcherPID, int signalsCnt, enum SendingMode sendingMode)
{
    for (int i = 0; i < signalsCnt; ++i)
    {
        switch (sendingMode)
        {
            case KILL:
            case SIGRT:
                kill(catcherPID, SEND_SIGNAL);
                break;
            case SIGQUEUE:
                sigval_t sigVal = {i};
                sigqueue(catcherPID, SEND_SIGNAL, sigVal);
                break;
        }
        ++signalsSentCnt;
    }

    switch (sendingMode)
    {
        case KILL:
        case SIGRT:
            kill(catcherPID, FINISH_SIGNAL);
            break;
        case SIGQUEUE:
            sigval_t sigVal = {signalsCnt};
            sigqueue(catcherPID, FINISH_SIGNAL, sigVal);
            break;
        ++signalsSentCnt;
    }
}

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        printf("[ERROR] Required parameters: catcherPID, signalsCnt, sendingMode\n");
        return -1;
    }

    int catcherPID = atoi(argv[1]);
    int signalsCnt = atoi(argv[2]);
    enum SendingMode sendingMode = strToSendingMode(argv[3]);

    if (sendingMode == SIGRT)
    {
        SEND_SIGNAL = SIGRTMIN;
        FINISH_SIGNAL = SIGRTMAX;
    }

    struct sigaction sendAct;
    setSigAction(sendAct, sendSignalHandler, SEND_SIGNAL, SA_SIGINFO);

    struct sigaction finishAct;
    setSigAction(finishAct, finishSignalHandler, FINISH_SIGNAL, SA_SIGINFO);

    sendSignals(catcherPID, signalsCnt, sendingMode);

    sigset_t sigSet;
    sigemptyset(&sigSet);
    sigaddset(&sigSet, FINISH_SIGNAL);
    sigsuspend(&sigSet);

    printf("Sent: %d, received: %d\n", signalsSentCnt, signalsReceivedCnt);

    return 0;
}
