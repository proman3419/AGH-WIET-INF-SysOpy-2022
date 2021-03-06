#ifndef COMMON
#define COMMON

#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#define MAX_CLIENTS 32
#define MAX_MESSAGE_LEN 500
#define MSG_TYPE_STR_LEN 4
#define SERVER_PROJ 's'
#define SERVER_CID 9999
#define PERMISSIONS 0600

enum MsgType
{
    STOP = 1,
    LIST = 2,
    TALL = 3,
    TONE = 4,
    INIT = 5,
    UNKN = 6
};

struct Mtext
{
    int qidFrom;
    size_t cidFrom;
    size_t cidTo;
    char msg[MAX_MESSAGE_LEN];
    struct tm time;
};

struct MsgBuf
{
    long mtype;
    struct Mtext mtext;
};
extern const size_t MSG_BUF_SIZE;

void printSendFail(enum MsgType msgType, size_t cidTo);
void printReceiveFail(enum MsgType msgType, size_t cidFrom);
void perrorAndExit();
char* msgTypeToStr(enum MsgType msgType);
enum MsgType strToMsgType(char* str);
enum MsgType extractMsgTypeFromMsg(char* msg);
char* timeToReadable(struct tm* time, char* timeBuf);
// assume that we always set time to the current one
void fillMsgBuf(struct MsgBuf* msgBuf, enum MsgType msgType, int qidFrom, 
                size_t cidFrom, size_t cidTo, char* msg);
void printMtext(struct Mtext* mtext);
void sigintHandlerSetup(void (*handlerFunc)(int));

#endif
