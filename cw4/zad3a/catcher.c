#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

enum SendingMode {KILL, SIGQUEUE, SIGRT};

int SEND_SIGNAL = SIGUSR1;
int FINISH_SIGNAL = SIGUSR2;
int senderPID;
int signalsReceivedCnt = 0;

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

void sendHandler(int signo, siginfo_t* info, void* context)
{
    if (signo != SEND_SIGNAL)
        return;

    ++signalsReceivedCnt;
    printf("Received a SEND_SIGNAL from sender\n");
}

void finishHandler(int signo, siginfo_t* info, void* context)
{
    if (signo != FINISH_SIGNAL)
        return;

    printf("Received a FINISH_SIGNAL from sender\n");
    senderPID = info->si_pid;
}

void sendSignals(int signalsCnt, enum SendingMode sendingMode)
{
    for (int i = 1; i <= signalsCnt; ++i)
    {
        switch (sendingMode)
        {
            case KILL:
            case SIGRT:
                kill(senderPID, SEND_SIGNAL);
                break;
            case SIGQUEUE:
                sigval_t sigVal = {i};
                sigqueue(senderPID, SEND_SIGNAL, sigVal);
                break;
        }
    }

    sigval_t sigVal = {signalsReceivedCnt};
    sigqueue(senderPID, FINISH_SIGNAL, sigVal);
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("[ERROR] Required parameters: sendingMode\n");
        return -1;
    }

    enum SendingMode sendingMode = strToSendingMode(argv[1]);

    printf("Catcher PID: %d\n", getpid());

    if (sendingMode == SIGRT)
    {
        SEND_SIGNAL = SIGRTMIN;
        FINISH_SIGNAL = SIGRTMAX;
    }

    struct sigaction sendAct;
    setSigAction(sendAct, sendHandler, SEND_SIGNAL, SA_SIGINFO);

    struct sigaction finishAct;
    setSigAction(finishAct, finishHandler, FINISH_SIGNAL, SA_SIGINFO);

    sigset_t sigSet;
    sigemptyset(&sigSet);
    sigaddset(&sigSet, FINISH_SIGNAL);
    sigsuspend(&sigSet);

    sendSignals(signalsReceivedCnt, sendingMode);

    return 0;
}
