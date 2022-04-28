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
    printf("%d %d\n", qid, qidServer);

    printf("Set up\n");
}

void stopHandler()
{

}

void listHandler()
{
    struct MsgBuf request;
    fillMsgBuf(&request, LIST, qkey, qid, cid, "");
    if (msgsnd(qidServer, &request, sizeof(struct MsgBuf), 0) == -1)
        perrorAndExit();

    struct MsgBuf response;
    if (msgrcv(qid, &response, sizeof(struct MsgBuf), LIST, 0) == -1)
        perrorAndExit();
    printf("%s\n", response.msg);
}

void tallHandler(char* args)
{

}

void toneHandler(char* args)
{
    // int ocid;
    // char* msg;
    // sscanf(args, "%d %[^\t\n]", &bb, msg);

    // struct MsgBuf request;
    // fillMsgBuf(&request, TONE, qkey, qid, ocid, "");
    // if (msgsnd(qidServer, &request, sizeof(struct MsgBuf), 0) == -1)
    //     perrorAndExit();

    // struct MsgBuf response;
    // if (msgrcv(qid, &response, sizeof(struct MsgBuf), TONE, 0) == -1)
    //     perrorAndExit();
    // printf("%s\n", response.msg);
}

void init()
{
    printf("Initializing...\n");
    struct MsgBuf request;
    fillMsgBuf(&request, INIT, qkey, qid, -1, "");
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

    for (;;)
    {
        char input[MAX_MESSAGE_LEN];
        fgets(input, MAX_MESSAGE_LEN, stdin);
        enum Job job = extractJobFromMsg(input);
        char* args = input + JOB_STR_LEN + 1; // +1 for space

        switch (job)
        {
            case STOP: stopHandler(); break;
            case LIST: listHandler(); break;
            case TALL: tallHandler(args); break;
            case TONE: toneHandler(args); break;
            default: break;
        }
    }

    return 0;
}
