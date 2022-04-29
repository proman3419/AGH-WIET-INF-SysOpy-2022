#include "common.h"

const size_t MSG_BUF_SIZE = sizeof(struct MsgBuf) - sizeof(long);

void perrorAndExit()
{
    perror("[!] Error");
    exit(-1);
}

enum MsgType strToMsgType(char* str)
{
    if (strcmp(str, "STOP") == 0) return STOP;
    if (strcmp(str, "LIST") == 0) return LIST;
    if (strcmp(str, "TALL") == 0) return TALL;
    if (strcmp(str, "TONE") == 0) return TONE;
    if (strcmp(str, "INIT") == 0) return INIT;
    if (strcmp(str, "TEXT") == 0) return TEXT;
    return UNKN;
}

char* msgTypeToStr(enum MsgType msgType)
{
    switch (msgType)
    {
        case STOP: return "STOP";
        case LIST: return "LIST";
        case TALL: return "TALL";
        case TONE: return "TONE";
        case INIT: return "INIT";
        case TEXT: return "TEXT";
        default: return "UNKN";
    }
}

enum MsgType extractMsgTypeFromMsg(char* msg)
{
    enum MsgType msgType = UNKN;
    if (strlen(msg) >= MSG_TYPE_STR_LEN)
    {
        char temp = msg[MSG_TYPE_STR_LEN];
        msg[MSG_TYPE_STR_LEN] = '\0';
        msgType = strToMsgType(msg);
        msg[MSG_TYPE_STR_LEN] = temp;
    }
    return msgType;
}

void fillMtext(struct Mtext* mtext, int qidFrom, int cidFrom, int cidTo, char* msg)
{
    mtext->qidFrom = qidFrom;
    mtext->cidFrom = cidFrom;
    mtext->cidTo = cidTo;
    strcpy(mtext->msg, msg);

    time_t rawTime;
    time(&rawTime);
    mtext->time = *localtime(&rawTime);
}

void printTime(const struct tm* time)
{
    printf("%s", asctime(time));
}
