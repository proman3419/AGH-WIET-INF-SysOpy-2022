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
    printf("Set up\n");
}

void stopHandler(struct MsgBuf received)
{

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
    fillMsgBuf(&response, LIST, qkey, qid, MAX_CLIENTS, "");
    strcpy(response.msg, msg);
    if (msgsnd(received.qid, &response, sizeof(struct MsgBuf), 0) == -1)
        printf("Sending response for LIST request to %d failed\n", received.qid);
}

void tallHandler(struct MsgBuf received)
{

}

void toneHandler(struct MsgBuf received)
{

}

void initHandler(struct MsgBuf received)
{
    int cid = -1;
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (qidsClients[i] == -1)
        {
            qidsClients[i] = received.qid;
            cid = i;
            break;
        }
    }

    struct MsgBuf response;
    if (cid != -1)
    {
        char cidStr[16];
        sprintf(cidStr, "%d", cid);
        fillMsgBuf(&response, INIT, qkey, qid, MAX_CLIENTS, cidStr);
    }
    else
        fillMsgBuf(&response, INIT, qkey, qid, MAX_CLIENTS, "-1");

    if (msgsnd(received.qid, &response, sizeof(struct MsgBuf), 0) == -1)
        printf("Sending response for INIT request to %d failed\n", received.qid);
}

int main(int argc, char** argv)
{
    setup();

    for (;;)
    {
        struct MsgBuf received;
        if (msgrcv(qid, &received, sizeof(struct MsgBuf), 0, IPC_NOWAIT) == -1)
        {
            if (errno != ENOMSG)
                perrorAndExit();
        }
        else
        {
            printf("Received request %s from %d\n", jobToStr((enum Job)received.mtype), received.cid);
            switch ((enum Job)received.mtype)
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

    return 0;
}
