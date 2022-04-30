#include "common.h"
#include "server.h"

int qidsClients[MAX_CLIENTS];
int qid;
key_t qkey;

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

    qkey = ftok(getenv("HOME"), SERVER_PROJ);
    if (qkey == -1)
        perrorAndExit();

    qid = msgget(qkey, IPC_CREAT | PERMISSIONS);
    if (qid == -1)
        perrorAndExit();

    for (int i = 0; i < MAX_CLIENTS; ++i)
        qidsClients[i] = -1;

    printf("My qid: %d\n", qid);

    printf("Set up\n");
}

void stopHandler(struct MsgBuf received)
{
    printf("Client %d disconnected\n", received.mtext.cidFrom);
    qidsClients[received.mtext.cidFrom] = -1;
}

void listHandler(struct MsgBuf received)
{
    char msg[MAX_MESSAGE_LEN] = "Active clients:\n";
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        char cidStr[16];
        if (qidsClients[i] != -1)
        {
            sprintf(cidStr, "%d\n", i);
            strcat(msg, cidStr);
        }
    }

    struct MsgBuf response;
    response.mtype = LIST;
    fillMtext(&response.mtext, qid, MAX_CLIENTS, received.mtext.cidFrom, msg);
    if (msgsnd(received.mtext.qidFrom, &response, MSG_BUF_SIZE, 0) == -1)
        printf("Sending response for LIST request to %d failed\n", received.mtext.cidFrom);
}

void tallHandler(struct MsgBuf received)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (qidsClients[i] != -1)
        {
            struct MsgBuf response;
            response.mtype = TEXT;
            fillMtext(&response.mtext, received.mtext.qidFrom, received.mtext.cidFrom, received.mtext.cidTo, received.mtext.msg);
            printf("%s to: %d from: %d\n", received.mtext.msg, qidsClients[i], received.mtext.qidFrom);
            if (msgsnd(qidsClients[i], &response, MSG_BUF_SIZE, 0) == -1)
                printf("Forwarding the message from %d to %d failed\n", received.mtext.cidFrom, received.mtext.cidTo);

        }
    }

    // if (received.cidTo < 0 || received.cidTo >= MAX_CLIENTS)
    // {
    //     printf("Invalid client ID\n");
    //     return;
    // }
    // if (qidsClients[received.cidTo] == -1)
    // {
    //     printf("The client with ID %d is not active\n", received.cidTo);
    //     return;        
    // }

    // if (msgsnd(qidsClients[received.cidTo], &received, MSG_BUF_SIZE, 0) == -1)
    //     printf("Forwarding the message from %d to %d failed\n", received.cidFrom, received.cidTo);
}

void toneHandler(struct MsgBuf received)
{
    printf("yzje\n");
    struct MsgBuf response;
    response.mtype = TEXT;
    fillMtext(&response.mtext, received.mtext.qidFrom, received.mtext.cidFrom, received.mtext.cidTo, received.mtext.msg);
    printf("%s to: %d from: %d\n", received.mtext.msg, qidsClients[received.mtext.cidTo], received.mtext.qidFrom);
    if (msgsnd(qidsClients[received.mtext.cidTo], &response, MSG_BUF_SIZE, 0) == -1)
        printf("Forwarding the message from %d to %d failed\n", received.mtext.cidFrom, received.mtext.cidTo);
}

void initHandler(struct MsgBuf received)
{
    int cid = -1;
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (qidsClients[i] == -1)
        {
            qidsClients[i] = received.mtext.qidFrom;
            cid = i;
            break;
        }
    }

    struct MsgBuf response;
    response.mtype = INIT;
    if (cid != -1)
    {
        char cidStr[16];
        sprintf(cidStr, "%d", cid);
        fillMtext(&response.mtext, qid, MAX_CLIENTS, received.mtext.cidFrom, cidStr);
    }
    else
        fillMtext(&response.mtext, qid, MAX_CLIENTS, received.mtext.cidFrom, "-1");

    if (msgsnd(received.mtext.qidFrom, &response, MSG_BUF_SIZE, 0) == -1)
        printf("Sending response for INIT request to %d failed\n", received.mtext.cidFrom);
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
            printf("Received request %s from %d\n", msgTypeToStr((enum MsgType)received.mtype), received.mtext.cidFrom);
            switch ((enum MsgType)received.mtype)
            {
                case STOP: printf("MAMY TO\n"); stopHandler(received); break;
                case LIST: listHandler(received); break;
                case TALL: tallHandler(received); break;
                case TONE: toneHandler(received); break;
                case INIT: initHandler(received); break;
                default: break;
            }
        }
    }

    return 0;
}
