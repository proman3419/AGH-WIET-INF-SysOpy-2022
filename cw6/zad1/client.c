#include "common.h"
#include "client.h"

int qid, qidServer;
key_t qkey, qkeyServer;
size_t cid;
pid_t pidSender, pidListener;

void clean()
{
    printf("Cleaning...\n");
    msgctl(qid, IPC_RMID, NULL);
    printf("Cleaned\n");
}

void setup()
{
    printf("Setting up...\n");
    atexit(clean);
    pidListener = getpid();
    sigintHandlerSetup();

    qkeyServer = ftok(getenv("HOME"), SERVER_PROJ);
    qkey = ftok(getenv("HOME"), getpid()%256);
    if (qkeyServer == -1 || qkey == -1)
        perrorAndExit();

    qidServer = msgget(qkeyServer, 0);
    qid = msgget(qkey, IPC_CREAT | IPC_EXCL | PERMISSIONS);
    if (qidServer == -1 || qid == -1)
        perrorAndExit();

    printf("My qid: %d\n", qid);
    printf("Server qid: %d\n", qidServer);

    printf("Set up\n");
}

void init()
{
    printf("Initializing...\n");
    struct MsgBuf request;
    request.mtype = INIT;
    fillMtext(&request.mtext, qid, -1, MAX_CLIENTS, "");
    if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
        perrorAndExit();

    struct MsgBuf response;
    if (msgrcv(qid, &response, MSG_BUF_SIZE, INIT, 0) == -1)
        perrorAndExit();
    cid = atoi(response.mtext.msg);
    printf("Initialized, the client ID is: %ld\n", cid);
}

void stopHandler()
{
    printf("SIEMA\n");
    struct MsgBuf request;
    request.mtype = STOP;
    fillMtext(&request.mtext, qid, cid, MAX_CLIENTS, "");
    printf("%d\n", qidServer);
    if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
    {
        printf("STOP SEND FAILED %d %d %d %d %d\n", errno, EAGAIN, EIDRM , EINTR, EINVAL);
    }
    exit(0);
}

void listHandler()
{
    struct MsgBuf request;
    request.mtype = LIST;
    fillMtext(&request.mtext, qid, cid, MAX_CLIENTS, "");
    if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
        perrorAndExit();

    struct MsgBuf response;
    if (msgrcv(qid, &response, MSG_BUF_SIZE, LIST, 0) == -1)
        perrorAndExit();
    printf("%s\n", response.mtext.msg);
}

void tallHandler(char* msg)
{
    struct MsgBuf request;
    request.mtype = TALL;
    fillMtext(&request.mtext, qid, cid, -1, msg);
    if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
        perrorAndExit();
}

void toneHandler(int cidTo, char* msg)
{
    struct MsgBuf request;
    request.mtype = TONE;
    fillMtext(&request.mtext, qid, cid, cidTo, msg);
    if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
        perrorAndExit();
}

void sigintHandler(int sigNum)
{
    printf("DOSTALEM\n");
    if (getpid() == pidListener)
        stopHandler();
    exit(0);
}

void sigintHandlerSetup()
{
    struct sigaction act;
    act.sa_handler = sigintHandler;
    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask, SIGINT);
    sigprocmask(SIG_SETMASK, &act.sa_mask, NULL);
    sigaction(SIGINT, &act, NULL);
}

void sender()
{
    char input[MAX_MESSAGE_LEN];
    char* rest;
    char* token;
    enum MsgType msgType;
    for (;;)
    {
        fgets(input, MAX_MESSAGE_LEN, stdin);
        msgType = extractMsgTypeFromMsg(input);

        switch (msgType)
        {
            case STOP:
                printf("%d\n", pidListener);
                kill(pidListener, SIGINT);
                stopHandler();
                break;
            case LIST: listHandler(); break;
            case TALL: 
                strtok_r(input, " ", &rest);  
                tallHandler(rest);
                break;
            case TONE:
                strtok_r(input, " ", &rest);  
                token = strtok_r(NULL, " ", &rest);
                toneHandler(atoi(token), rest);
                break;
            default: break;
        }
    }
}

void listener()
{
    struct MsgBuf received;
    for (;;)
    {
        if (msgrcv(qid, &received, MSG_BUF_SIZE, TEXT, 0) == -1)
        {
            if (errno != ENOMSG)
                perrorAndExit();
        }
        else
        {
            printf("Received request %s from %d\n", msgTypeToStr((enum MsgType)received.mtype), received.mtext.cidFrom);
            switch ((enum MsgType)received.mtype)
            {
                case STOP: 
                    printf("Received STOP from server\n");
                    kill(pidSender, SIGINT);
                    exit(0);
                    break;
                case LIST: printf("list resp\n"); break;
                case TEXT: printf("%s\n", received.mtext.msg); break;
                case INIT: printf("init resp\n"); break;
                default: break;
            }
        }
    }    
}

int main(int argc, char** argv)
{
    setup();
    init();

    if ((pidSender = fork()) == 0)
        sender();
    else
        listener();        

    return 0;
}
