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

// void stopHandler()
// {

// }

// void listHandler()
// {
//     struct MsgBuf request;
//     fillMsgBuf(&request, LIST, qid, cid, MAX_CLIENTS, "");
//     if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
//         perrorAndExit();

//     struct MsgBuf response;
//     if (msgrcv(qid, &response, MSG_BUF_SIZE, LIST, 0) == -1)
//         perrorAndExit();
//     printf("%s\n", response.mtext);
// }

// void tallHandler(char* args)
// {

// }

void toneHandler(int cidTo, char* mtext)
{
    printf("cidto: %d | mtext: %s\n", cidTo, mtext);
    // int ocid;
    // char* mtext;
    // sscanf(args, "%d %[^\t\n]", &bb, mtext);

    // struct MsgBuf request;
    // fillMsgBuf(&request, TONE, qkey, qid, ocid, "");
    // if (msgsnd(qidServer, &request, MSG_BUF_SIZE, 0) == -1)
    //     perrorAndExit();

    // struct MsgBuf response;
    // if (msgrcv(qid, &response, MSG_BUF_SIZE, TONE, 0) == -1)
    //     perrorAndExit();
    // printf("%s\n", response.mtext);
}

int main(int argc, char** argv)
{
    setup();
    init();

    char input[MAX_MESSAGE_LEN];
    char* rest;
    char* token;
    enum Job job;
    for (;;)
    {
        fgets(input, MAX_MESSAGE_LEN, stdin);
        job = extractJobFromMsg(input);
        // args = input + JOB_STR_LEN + 1; // +1 for space
        // strtok_r(input, " ", &rest);  
        // token = strtok_r(NULL, " ", &rest);

        switch (job)
        {
            // case STOP: stopHandler(); break;
            // case LIST: listHandler(); break;
            // case TALL: tallHandler(args); break;
            case TONE:
                strtok_r(input, " ", &rest);  
                token = strtok_r(NULL, " ", &rest);
                toneHandler(atoi(token), rest);
                break;
            default: break;
        }
    }

    return 0;
}
