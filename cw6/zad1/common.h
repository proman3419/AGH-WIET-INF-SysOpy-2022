#ifndef COMMON
#define COMMON

#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX_CLIENTS 32
#define MAX_MESSAGE_LEN 512
#define JOB_STR_LEN 4
#define SERVER_PROJ 's'
#define PERMISSIONS 0600

enum Job
{
    STOP,
    LIST,
    TALL,
    TONE,
    INIT,
    UNKN
};

struct MsgBuf
{
    long mtype;
    key_t qkey;
    int qid;
    int cid;
    char msg[MAX_MESSAGE_LEN];
};

void perrorAndExit();
enum Job strToJob(char* str);
char* jobToStr(enum Job job);
enum Job extractJobFromMsg(char* msg);
void fillMsgBuf(struct MsgBuf* msgBuf, long mtype, 
                key_t qkey, int qid, int cid, char* msg);

#endif
