#include "common.h"

void perrorAndExit()
{
    perror("[!] Error");
    exit(-1);
}

enum Job strToJob(char* str)
{
    if (strcmp(str, "STOP") == 0) return STOP;
    if (strcmp(str, "LIST") == 0) return LIST;
    if (strcmp(str, "TALL") == 0) return TALL;
    if (strcmp(str, "TONE") == 0) return TONE;
    if (strcmp(str, "INIT") == 0) return INIT;
    return UNKN;
}

char* jobToStr(enum Job job)
{
    switch (job)
    {
        case STOP: return "STOP";
        case LIST: return "LIST";
        case TALL: return "TALL";
        case TONE: return "TONE";
        case INIT: return "INIT";
        default: return "UNKN";
    }
}

enum Job extractJobFromMsg(char* msg)
{
    enum Job job = UNKN;
    if (strlen(msg) >= JOB_STR_LEN)
    {
        char temp = msg[JOB_STR_LEN];
        msg[JOB_STR_LEN] = '\0';
        job = strToJob(msg);
        msg[JOB_STR_LEN] = temp;
    }
    return job;
}

void fillMsgBuf(struct MsgBuf* msgBuf, long mtype, 
                key_t qkey, int qid, int cid, char* msg)
{
    if (mtype == -1)
    {
        msgBuf->mtype = extractJobFromMsg(msg);
        msg = msg + JOB_STR_LEN;
    }
    else
        msgBuf->mtype = mtype;
    msgBuf->qkey = qkey;
    msgBuf->qid = qid;
    msgBuf->cid = cid;
    strcpy(msgBuf->msg, msg);
}
