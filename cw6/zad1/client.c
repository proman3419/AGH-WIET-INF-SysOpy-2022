#include "common.h"
#include "client.h"

int qid, qidServer;
key_t qkey, qkeyServer;
size_t cid;

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

    qkeyServer = ftok(getenv("HOME"), SERVER_PROJ);
    qkey = ftok(getenv("HOME"), getpid()%256);
    if (qkeyServer == -1 || qkey == -1)
        perrorAndExit();

    qidServer = msgget(qkeyServer, 0);
    qid = msgget(qkey, IPC_CREAT | IPC_EXCL | PERMISSIONS);
    if (qidServer == -1 || qid == -1)
        perrorAndExit();
    printf("Set up\n");
}

void stopHandler()
{

}

void listHandler()
{
    struct MsgBuf request;
    fillMsgBuf(&request, LIST, qid, cid, MAX_CLIENTS, "");
    if (msgsnd(qidServer, &request, sizeof(struct MsgBuf), 0) == -1)
        perrorAndExit();

    struct MsgBuf response;
    if (msgrcv(qid, &response, sizeof(struct MsgBuf), LIST, 0) == -1)
        perrorAndExit();
    printf("%s\n", response.msg);
}

void tallHandler(char* input)
{

}

void toneHandler(int cidTo, char* msg)
{
    printf("cid %d | msg %s\n", cidTo, msg);

    // struct MsgBuf request;
    // fillMsgBuf(&request, TONE, qid, cid, cidTo, msg);
    // if (msgsnd(qidServer, &request, sizeof(struct MsgBuf), 0) == -1)
    //     perrorAndExit();
}

void init()
{
    printf("Initializing...\n");
    struct MsgBuf request;
    fillMsgBuf(&request, INIT, qid, -1, MAX_CLIENTS, "");
    if (msgsnd(qidServer, &request, sizeof(struct MsgBuf), 0) == -1)
        perrorAndExit();

    struct MsgBuf response;
    if (msgrcv(qid, &response, sizeof(struct MsgBuf), INIT, 0) == -1)
        perrorAndExit();
    cid = atoi(response.msg);
    printf("Initialized, the client ID is: %ld\n", cid);
}

int main(int argc, char** argv)
{
    setup();
    init();

    char input[MAX_MESSAGE_LEN];
    char* cidToStr;
    char* msg = NULL;
    for (;;)
    {
        fgets(input, MAX_MESSAGE_LEN, stdin);
        strtok_r(input, " ", &msg);

        // enum Job job = extractJobFromMsg(input);

        // switch (job)
        // {
        //     // case STOP: stopHandler(); break;
        //     // case LIST: listHandler(); break;
        //     // case TALL: tallHandler(input); break;
        //     case TONE:
        //         printf("%s\n", input);
        //         strtok_r(input, " ", &msg);
        //         // if ((cidToStr = strtok_r(msg, " ", &msg)) != NULL)
        //             // toneHandler(atoi(cidToStr), msg);
        //         break;
        //     default: break;
        // }
    }

    return 0;
}
