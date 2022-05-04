#include "common.h"
#include "client.h"

int qid, qidServer;
key_t qkey, qkeyServer;
size_t cid;
pid_t pidSender, pidListener;

void clean()
{
    msgctl(qid, IPC_RMID, NULL);
}

void sigintHandler(int sigNum)
{
    if (getpid() == pidSender)
        stopHandler();
    exit(0);
}

void setup()
{
    printf("Setting up...\n");
    atexit(clean);
    sigintHandlerSetup(sigintHandler);

    qkeyServer = ftok(getenv("HOME"), SERVER_PROJ);
    qkey = ftok(getenv("HOME"), getpid());
    if (qkeyServer == -1 || qkey == -1)
        perrorAndExit();

    qidServer = msgget(qkeyServer, 0);
    qid = msgget(qkey, IPC_CREAT | PERMISSIONS);
    if (qidServer == -1)
    {
        printf("Server is offline\n");
        exit(-1);
    }
    if (qid == -1)
        perrorAndExit();
    printf("Set up\n");
}

void init()
{
    printf("Initializing...\n");
    struct MsgBuf request;
    fillMsgBuf(&request, INIT, qid, -1, SERVER_CID, "");
    if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
        perrorAndExit();

    struct MsgBuf response;
    if (msgrcv(qid, &response, MSG_BUF_SIZE, INIT, 0) == -1)
        perrorAndExit();
    cid = response.mtext.cidTo;
    if (cid == -1)
    {
        printf("Max clients connected to the server\n");
        exit(0);
    }
    printf("Initialized, the client ID is: %ld\n", cid);
}

void stopHandler()
{
    struct MsgBuf request;
    fillMsgBuf(&request, STOP, qid, cid, SERVER_CID, "");
    if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
        printSendFail(STOP, SERVER_CID);
    exit(0);
}

void listHandler()
{
    struct MsgBuf request;
    fillMsgBuf(&request, LIST, qid, cid, SERVER_CID, "");
    if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
        printSendFail(LIST, SERVER_CID);
}

void tallHandler(char* msg)
{
    struct MsgBuf request;
    fillMsgBuf(&request, TALL, qid, cid, -1, msg);
    if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
        printSendFail(TALL, SERVER_CID);
}

void toneHandler(size_t cidTo, char* msg)
{
    struct MsgBuf request;
    fillMsgBuf(&request, TONE, qid, cid, cidTo, msg);
    if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
        printSendFail(TONE, SERVER_CID);
}

void sender()
{
    char input[MAX_MESSAGE_LEN];
    char* rest;
    char* token;
    enum MsgType msgType;
    for (;;)
    {
        printf("[CLIENT %ld]: ", cid);
        fgets(input, MAX_MESSAGE_LEN, stdin);
        msgType = extractMsgTypeFromMsg(input);

        switch (msgType)
        {
            case STOP:
                kill(pidListener, SIGINT);
                exit(0);
                break;
            case LIST: 
                listHandler();
                break;
            case TALL: 
                strtok_r(input, " ", &rest);
                rest[strlen(rest)-1] = '\0';
                tallHandler(rest);
                break;
            case TONE:
                strtok_r(input, " ", &rest);  
                token = strtok_r(NULL, " ", &rest);
                rest[strlen(rest)-1] = '\0';
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
        if (msgrcv(qid, &received, MSG_BUF_SIZE, 0, 0) == -1)
        {
            if (errno != ENOMSG)
                perrorAndExit();
        }
        else
        {
            switch ((enum MsgType)received.mtype)
            {
                case STOP: 
                    printf("\nServer disconnected");
                    kill(pidSender, SIGINT);
                    exit(0);
                    break;
                case LIST:
                    printf("\n### Clients online:\n%s", received.mtext.msg);
                    break;
                case TALL:
                case TONE:
                    printMtext(&received.mtext);
                    break;
                default: break;
            }
        }
    }    
}

int main(int argc, char** argv)
{
    setup();
    init();

    if (fork() == 0)
    {
        pidListener = getpid();
        listener();        
    }
    else
    {
        pidSender = getpid();
        sender();        
    }

    exit(0);
    return 0;
}
