#ifndef COMMON
#define COMMON

#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define MAX_MSG_LEN 256

struct Client {
    char* name;
    int fd;
    bool available;
    int opponent_idx;
};

enum GameState {
    START,
    WAIT_FOR_OPPONENT,
    WAIT_FOR_MOVE,
    OPPONENT_MOVE,
    MOVE,
    QUIT
};

enum CellOccupation {
    FREE,
    O,
    X
};

struct GameBoard {
    int move;
    enum CellOccupation objects[9];
};

#endif
