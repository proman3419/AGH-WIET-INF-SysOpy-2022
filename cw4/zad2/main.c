#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

#define SIGNAL SIGUSR1
#define CALLS_CNT 4

int callId;
int callDepth;

void setSigAction(struct sigaction act, void (*handlerFunc)(int, siginfo_t*, void*), int sigNo, int flag)
{
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handlerFunc;
    act.sa_flags = flag;
    sigaction(sigNo, &act, NULL);
    callId = 0;
    callDepth = 0;
}

void signalingHandler(int signo, siginfo_t* info, void* context)
{
    int currCallId = callId;
    printf("[START] Current call id: %d, call depth: %d\n", currCallId, callDepth);
    
    ++callId;
    ++callDepth;
    if (callId < CALLS_CNT)
        kill(getpid(), SIGNAL);
    --callDepth;

    printf("[END] Current call id: %d, call depth: %d\n", currCallId, callDepth);
}

void printingHandler(int signo, siginfo_t* info, void* context)
{
    printf("Signal number: %d\n", info->si_signo);
    printf("Signal value: %d\n", info->si_value.sival_int);
    printf("Sending process' PID: %d\n", info->si_pid);
    printf("Exit value / signal for process termination: %d\n", info->si_status);
    printf("Address at which fault occured: %p\n", info->si_addr);
}

void testNODEFER(struct sigaction act)
{
    printf("=====================[ SA_NODEFER ]=====================\n");

    setSigAction(act, signalingHandler, SIGNAL, SA_NODEFER);
    kill(getpid(), SIGNAL);
}

void testSIGINFO(struct sigaction act)
{
    printf("=====================[ SA_SIGINFO ]=====================\n");

    printf(">>> Send directly\n");
    setSigAction(act, printingHandler, SIGNAL, SA_SIGINFO);
    kill(getpid(), SIGNAL);
    printf("\n");

    printf(">>> Send from child\n");
    if (fork() == 0)
    {
        kill(getpid(), SIGNAL);
        exit(0);
    }
    else
        wait(NULL);
    printf("\n");

    printf(">>> Send with a custom signal value\n");
    setSigAction(act, printingHandler, SIGNAL, SA_SIGINFO);
    sigval_t sigVal = {1117};
    sigqueue(getpid(), SIGNAL, sigVal);
}

void testRESETHAND(struct sigaction act)
{
    printf("=====================[ SA_RESETHAND ]=====================\n");

    setSigAction(act, signalingHandler, SIGNAL, SA_RESETHAND);
    kill(getpid(), SIGNAL);
    kill(getpid(), SIGNAL);
}

int main(int argc, char** argv)
{
    struct sigaction act;
    testNODEFER(act);
    printf("\n");
    testSIGINFO(act);
    printf("\n");
    testRESETHAND(act);

    return 0;
}
