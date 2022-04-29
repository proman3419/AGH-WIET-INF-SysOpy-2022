#ifndef COMMON
#define COMMON

#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

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
    int qidFrom;
    int cidFrom;
    int cidTo;
    char msg[MAX_MESSAGE_LEN];
    struct tm time;
};

void perrorAndExit();
enum Job strToJob(char* str);
char* jobToStr(enum Job job);
enum Job extractJobFromMsg(char* msg);
void fillMsgBuf(struct MsgBuf* msgBuf, long mtype, int qidFrom,
                int cidFrom, int cidTo, char* msg);
                // assume that we always set time to the current one

#endif
