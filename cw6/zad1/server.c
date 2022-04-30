#include "common.h"
#include "server.h"

int qidsClients[MAX_CLIENTS];
int qid;
key_t qkey;
int clientsCnt;

void clean()
{
    msgctl(qid, IPC_RMID, NULL);
}

void sigintHandler(int sigNum)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (qidsClients[i] != -1)
        {
            struct MsgBuf request;
            fillMsgBuf(&request, STOP, qid, SERVER_CID, i, "");
            if (msgsnd(qidsClients[i], &request, MSG_BUF_SIZE, 0) == -1)
                printSendFail(STOP, i);
        }
    }

    struct MsgBuf received;
    do
    {
        if (msgrcv(qid, &received, MSG_BUF_SIZE, STOP, IPC_NOWAIT) == -1)
        {
            if (errno != ENOMSG)
                perrorAndExit();
        }
        else
        {
            stopHandler(received);
            logToFile(&received.mtext.time, received.mtext.cidFrom, 
                      received.mtype, received.mtext.msg);
        }
    } while (clientsCnt > 0); 

    printf("All clients disconnected, exiting");
    exit(0);
}

void setup()
{
    printf("Setting up...\n");
    atexit(clean);
    sigintHandlerSetup(sigintHandler);

    qkey = ftok(getenv("HOME"), SERVER_PROJ);
    if (qkey == -1)
        perrorAndExit();

    qid = msgget(qkey, IPC_CREAT | PERMISSIONS);
    if (qid == -1)
        perrorAndExit();

    for (int i = 0; i < MAX_CLIENTS; ++i)
        qidsClients[i] = -1;
    printf("Set up\n");
}

void stopHandler(struct MsgBuf received)
{
    printf("Client %ld disconnected\n", received.mtext.cidFrom);
    qidsClients[received.mtext.cidFrom] = -1;
    --clientsCnt;
}

void listHandler(struct MsgBuf received)
{
    char msg[MAX_MESSAGE_LEN] = "";
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        char cidStr[16];
        if (qidsClients[i] != -1)
        {
            sprintf(cidStr, "### %d\n", i);
            strcat(msg, cidStr);
        }
    }

    struct MsgBuf response;
    fillMsgBuf(&response, LIST, qid, MAX_CLIENTS, received.mtext.cidFrom, msg);
    if (msgsnd(received.mtext.qidFrom, &response, MSG_BUF_SIZE, 0) == -1)
        printSendFail(LIST, response.mtext.cidTo);
    else
        printf("Sent online clients list to %ld\n", response.mtext.cidTo);
}

void tallHandler(struct MsgBuf received)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (i != received.mtext.cidFrom && qidsClients[i] != -1)
        {
            struct MsgBuf response;
            fillMsgBuf(&response, TALL, received.mtext.qidFrom, 
                       received.mtext.cidFrom, -1, received.mtext.msg);
            if (msgsnd(qidsClients[i], &response, MSG_BUF_SIZE, 0) == -1)
                printSendFail(TALL, response.mtext.cidTo);
        }
    }
    printf("Forwarded message from %ld to all\n", received.mtext.cidFrom);
}

void toneHandler(struct MsgBuf received)
{
    struct MsgBuf response;
    fillMsgBuf(&response, TONE, received.mtext.qidFrom, received.mtext.cidFrom, received.mtext.cidTo, received.mtext.msg);
    if (msgsnd(qidsClients[received.mtext.cidTo], &response, MSG_BUF_SIZE, 0) == -1)
        printSendFail(TONE, response.mtext.cidTo);
    else
        printf("Forwarded message from %ld to %ld\n", received.mtext.cidFrom, received.mtext.cidTo);
}

void initHandler(struct MsgBuf received)
{
    int cidRegistered = -1;
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (qidsClients[i] == -1)
        {
            cidRegistered = i;
            break;
        }
    }

    struct MsgBuf response;
    fillMsgBuf(&response, INIT, qid, SERVER_CID, cidRegistered, "");
    if (msgsnd(received.mtext.qidFrom, &response, MSG_BUF_SIZE, 0) == -1)
        printSendFail(INIT, response.mtext.cidTo);
    else
    {
        printf("Registered new client as %ld\n", response.mtext.cidTo);
        qidsClients[cidRegistered] = received.mtext.qidFrom;
        ++clientsCnt;
    }
}

void logToFile(struct tm* time, size_t cidFrom, enum MsgType msgType, char* msg)
{
    FILE* logFilePtr;
    char timeBuf[32];
    if ((logFilePtr = fopen("log.log", "a+")) == NULL)
        perrorAndExit();
    fprintf(logFilePtr, "%s|%ld|%s|%s\n", 
            timeToReadable(time, timeBuf), cidFrom, msgTypeToStr(msgType), msg);
    fclose(logFilePtr);
}

int main(int argc, char** argv)
{
    setup();

    struct MsgBuf received;
    for (;;)
    {
        if (msgrcv(qid, &received, MSG_BUF_SIZE, 0, IPC_NOWAIT) == -1)
        {
            if (errno != ENOMSG)
                perrorAndExit();
        }
        else
        {
            printf("Received request %s from %ld\n", msgTypeToStr(received.mtype), 
                   received.mtext.cidFrom);
            logToFile(&received.mtext.time, received.mtext.cidFrom, 
                      received.mtype, received.mtext.msg);
            switch ((enum MsgType)received.mtype)
            {
                case STOP: stopHandler(received); break;
                case LIST: listHandler(received); break;
                case TALL: tallHandler(received); break;
                case TONE: toneHandler(received); break;
                case INIT: initHandler(received); break;
                default: break;
            }
        }
    }

    exit(0);
    return 0;
}
