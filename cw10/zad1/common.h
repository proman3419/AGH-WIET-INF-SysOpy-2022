#ifndef COMMON
#define COMMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 16
#define MAX_MSG_LEN 256

struct Client
{
    char* name;
    int fd;
    int available;
};

enum GameState
{
    START,
    WAIT_FOR_MOVE,
    WAIT_FOR_OPPONENT,
    OPPONENT_MOVE,
    MOVE,
    QUIT
};

enum CellOccupation
{
    NONE,
    O,
    X
};

struct GameBoard
{
    int move;
    enum CellOccupation objects[9];
};

#endif
