#include "common.h"

int serverSock;
int isFirstClient;
char buff[MAX_MSG_LEN+1];
char *name, *command, *arg;
struct GameBoard gameBoard;
enum GameState gameState = START;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int move(struct GameBoard *gameBoard, int position)
{
    if (position < 0 || position > 9 || gameBoard->objects[position] != FREE) return 0;

    gameBoard->objects[position] = gameBoard->move ? O : X;
    gameBoard->move = !gameBoard->move;
    return 1;
}

enum CellOccupation checkWin(struct GameBoard *gameBoard)
{
    enum CellOccupation column = FREE;

    for (int x = 0; x < 3; ++x)
    {
        enum CellOccupation first = gameBoard->objects[x];
        enum CellOccupation second = gameBoard->objects[x+3];
        enum CellOccupation third = gameBoard->objects[x+6];

        if (first == second && first == third && first != FREE)
            column = first;
    }

    if (column != FREE)
        return column;

    enum CellOccupation row = FREE;
    for (int y = 0; y < 3; ++y)
    {
        enum CellOccupation first = gameBoard->objects[3*y];
        enum CellOccupation second = gameBoard->objects[3*y+1];
        enum CellOccupation third = gameBoard->objects[3*y+2];
        if (first == second && first == third && first != FREE)
            row = first;
    }

    if (row != FREE)
        return row;

    enum CellOccupation lowerDiag = FREE;

    enum CellOccupation first = gameBoard->objects[0];
    enum CellOccupation second = gameBoard->objects[4];
    enum CellOccupation third = gameBoard->objects[8];

    if (first == second && first == third && first != FREE)
        lowerDiag = first;
    if (lowerDiag != FREE)
        return lowerDiag;

    enum CellOccupation upperDiag = FREE;
    first = gameBoard->objects[2];
    second = gameBoard->objects[4];
    third = gameBoard->objects[6];
    if (first == second && first == third && first != FREE)
        upperDiag = first;

    return upperDiag;
}

void end()
{
    sprintf(buff, "end: :%s", name);
    send(serverSock, buff, MAX_MSG_LEN, 0);
    exit(0);
}

void checkState()
{
    bool win = false;
    enum CellOccupation winner = checkWin(&gameBoard);

    if (winner != FREE)
    {
        if ((isFirstClient && winner == O) || (!isFirstClient && winner == X)) printf("You won!\n");
        else printf("You lost!\n");

        win = true;
    }

    bool draw = true;
    for (int i = 0; i < 9; i++)
    {
        if (gameBoard.objects[i] == FREE)
        {
            draw = false;
            break;
        }
    }

    if (draw && !win) printf("It's a draw!\n");

    if (win || draw) gameState = QUIT;
}

void parseCommand(char* msg)
{
    command = strtok(msg, ":");
    arg = strtok(NULL, ":");
}

struct GameBoard createBoard()
{
    struct GameBoard gameBoard = {1, {FREE}};
    return gameBoard;
}

void draw(){
    char symbol;

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            if (gameBoard.objects[y*3+x] == FREE)
                symbol = y * 3 + x + 1 + '0';
            else if (gameBoard.objects[y*3+x] == O)
                symbol = 'O';
            else symbol = 'X';

            printf("  %c  ", symbol);
        }
        printf("\n_________________________\n");
    }
}

void play()
{
    while (true)
    {
        if (gameState == START)
        {
            if (strcmp(arg, "name_taken") == 0)
                exit(1);
            else if (strcmp(arg, "no_enemy") == 0)
                gameState = WAIT_FOR_OPPONENT;
            else
            {
                gameBoard = createBoard();
                isFirstClient = arg[0] == 'O';
                gameState = isFirstClient ? MOVE : WAIT_FOR_MOVE;
            }
        }

        else if (gameState == WAIT_FOR_OPPONENT)
        {
            pthread_mutex_lock(&mutex);

            while (gameState != START && gameState != QUIT)
                pthread_cond_wait(&cond, &mutex);

            pthread_mutex_unlock(&mutex);

            gameBoard = createBoard();
            isFirstClient = arg[0] == 'O';
            gameState = isFirstClient ? MOVE : WAIT_FOR_MOVE;
        }
        else if (gameState == WAIT_FOR_MOVE)
        {
            printf("Waiting for rivals move\n");

            pthread_mutex_lock(&mutex);
            while (gameState != OPPONENT_MOVE && gameState != QUIT)
                pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
        else if (gameState == OPPONENT_MOVE)
        {
            int pos = atoi(arg);
            move(&gameBoard, pos);
            checkState();
            if (gameState != QUIT)
                gameState = MOVE;
        }
        else if (gameState == MOVE)
        {
            draw();

            int pos;
            do
            {
                printf("Next move (%c): ", isFirstClient ? 'O' : 'X');
                scanf("%d", &pos);
                pos--;
            } while (!move(&gameBoard, pos));

            draw();

            char buff[MAX_MSG_LEN + 1];
            sprintf(buff, "move:%d:%s", pos, name);
            send(serverSock, buff, MAX_MSG_LEN, 0);

            checkState();
            if (gameState != QUIT)
                gameState = WAIT_FOR_MOVE;
        }
        else if (gameState == QUIT) end();
    }
}

void connectLocal(char* path){
    serverSock = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(struct sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, path);

    if (connect(serverSock, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == -1)
    {
        printf("Error while connecting to LOCAL socket (%s)\n", strerror(errno));
        exit(1);
    }
}

void connectInet(char* port)
{
    struct addrinfo *info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo("localhost", port, &hints, &info);

    serverSock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

    if (connect(serverSock, info->ai_addr, info->ai_addrlen) == -1)
    {
        printf("Error while connecting to INET socket (%s)\n", strerror(errno));
        exit(1);
    }

    freeaddrinfo(info);
}

void listenServer()
{
    int gameThreadRunning = 0;
    while (1)
    {
        recv(serverSock, buff, MAX_MSG_LEN, 0);
        parseCommand(buff);

        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0)
        {
            gameState = START;
            if (!gameThreadRunning)
            {
                pthread_t t;
                pthread_create(&t, NULL, (void *(*)(void *))play, NULL);
                gameThreadRunning = 1;
            }
        }
        else if (strcmp(command, "move") == 0)
        {
            gameState = OPPONENT_MOVE;
        }
        else if (strcmp(command, "end") == 0)
        {
            gameState = QUIT;
            exit(0);
        }
        else if (strcmp(command, "ping") == 0)
        {
            sprintf(buff, "pong: :%s", name);
            send(serverSock, buff, MAX_MSG_LEN, 0);
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        printf("Wrong number of arguments!\n");
        exit(1);
    }

    name = argv[1];

    // handle SIGINT
    signal(SIGINT, end);

    // connection method - inet/unix
    if (strcmp(argv[2], "unix") == 0)
    {
        char* path = argv[3];
        connectLocal(path);
    }

    else if (strcmp(argv[2], "inet") == 0)
    {
        char* port = argv[3];
        connectInet(port);
    }

    else
    {
        printf("Wrong method - choose [inet] or [unix]!\n");
        exit(1);
    }

    char msg[MAX_MSG_LEN];
    sprintf(msg, "add: :%s", name);
    send(serverSock, msg, MAX_MSG_LEN, 0);

    listenServer();

    return 0;
}
