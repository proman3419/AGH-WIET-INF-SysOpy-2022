#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void sighandler(int signo, siginfo_t* info, void* context)
{
    printf("%d\n", info->si_value.sival_int);
}

int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = &sighandler;

    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);

    sigset_t sigSet;
    sigfillset(&sigSet);
    sigdelset(&sigSet, SIGUSR1);
    sigdelset(&sigSet, SIGUSR2);
    sigprocmask(SIG_SETMASK, &sigSet, NULL);

    int child = fork();
    if(child == 0) {
        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1 i SIGUSR2
        //zdefiniuj obsluge SIGUSR1 i SIGUSR2 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalami SIGUSR1 i SIGUSR2 wartosci
        sleep(1);
    }
    else {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
        sigval_t sigVal = {atoi(argv[1])};
        sigqueue(child, atoi(argv[2]), sigVal);
        while (wait(NULL) < 0);
    }

    return 0;
}
