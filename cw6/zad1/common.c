#include "common.h"

const size_t MSG_BUF_SIZE = sizeof(struct MsgBuf) - sizeof(long);

void printSendFail(enum MsgType msgType, size_t cidTo)
{
    printf("Failed to send %s to %ld\n", msgTypeToStr(msgType), cidTo);
}

void printReceiveFail(enum MsgType msgType, size_t cidFrom)
{
    printf("Failed to receive %s from %ld\n", msgTypeToStr(msgType), cidFrom);
}

void perrorAndExit()
{
    perror("Fatal error");
    exit(-1);
}

char* msgTypeToStr(enum MsgType msgType)
{
    switch (msgType)
    {
        case STOP: return "STOP";
        case LIST: return "LIST";
        case TALL: return "2ALL";
        case TONE: return "2ONE";
        case INIT: return "INIT";
        default: return "UNKN";
    }
}

enum MsgType strToMsgType(char* str)
{
    if (strcmp(str, "STOP") == 0) return STOP;
    if (strcmp(str, "LIST") == 0) return LIST;
    if (strcmp(str, "2ALL") == 0) return TALL;
    if (strcmp(str, "2ONE") == 0) return TONE;
    if (strcmp(str, "INIT") == 0) return INIT;
    return UNKN;
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

char* timeToReadable(struct tm* time, char* timeBuf)
{
    timeBuf = asctime(time);
    timeBuf[strlen(timeBuf)-1] = '\0';
    return timeBuf;
}

void fillMsgBuf(struct MsgBuf* msgBuf, enum MsgType mtype, int qidFrom, 
                size_t cidFrom, size_t cidTo, char* msg)
{
    msgBuf->mtype = mtype;
    msgBuf->mtext.qidFrom = qidFrom;
    msgBuf->mtext.cidFrom = cidFrom;
    msgBuf->mtext.cidTo = cidTo;
    strcpy(msgBuf->mtext.msg, msg);

    time_t rawTime;
    time(&rawTime);
    msgBuf->mtext.time = *localtime(&rawTime);
}

void printMtext(struct Mtext* mtext)
{
    char timeBuf[32];
    printf("\n");
    printf("### From: %ld\n", mtext->cidFrom);
    printf("### To: %ld\n", mtext->cidTo);
    printf("### Time: %s\n", timeToReadable(&mtext->time, timeBuf));
    printf("### Message: %s\n", mtext->msg);
}

void sigintHandlerSetup(void (*handlerFunc)(int))
{
    struct sigaction act;
    act.sa_handler = handlerFunc;
    sigfillset(&act.sa_mask);
    sigdelset(&act.sa_mask, SIGINT);
    sigprocmask(SIG_SETMASK, &act.sa_mask, NULL);
    sigaction(SIGINT, &act, NULL);
}
