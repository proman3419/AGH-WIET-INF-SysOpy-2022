#include "common.h"

struct Client *clients[MAX_CLIENTS] = {NULL};
int clientsCnt = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int setLocalSocket(char* sockPath)
{
    int sockFd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sockFd  == -1)
    {
        printf("[ERROR] Couldn't create local socket\n");
        exit(-1);
    }
    else
        printf("Local socket created\n");

    struct sockaddr_un sockAddr;
    memset(&sockAddr, 0, sizeof(struct sockaddr_un));
    sockAddr.sun_family = AF_UNIX;
    strcpy(sockAddr.sun_path, sockPath);

    // unlink the file so the bind will succeed
    unlink(sockPath);

    // binds address to socket
    if ((bind(sockFd, (struct sockaddr *) &sockAddr, sizeof(sockAddr))) == -1)
    {
        printf("[ERROR] Couldn't bind local socket\n");
        exit(-1);
    }

    if ((listen(sockFd, MAX_CLIENTS)) == -1)
    {
        printf("[ERROR] Listening on local socket failed\n");
        exit(-1);
    }

    printf("Local socket fd: %d\n", sockFd);

    return sockFd;
}

int setNetworkSocket(char *port)
{
    // create socket
    int sockFd = socket(AF_INET , SOCK_STREAM, 0);

    if (sockFd  == -1)
    {
        printf("[ERROR] Couldn't create inet socket\n");
        exit(-1);
    }
    else
        printf("Inet socket created\n");

    struct sockaddr_in sockAddr;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(atoi(port));

    // binds address to socket
    if ((bind(sockFd, (struct sockaddr *) &sockAddr, sizeof(sockAddr))) == -1)
    {
        printf("[ERROR] Couldn't bind inet socket\n");
        exit(-1);
    }

    if ((listen(sockFd, MAX_CLIENTS)) == -1)
    {
        printf("[ERROR] Listening on inet socket failed\n");
        exit(-1);
    }

    printf("Inet socket fd: %d\n", sockFd);

    return sockFd;
}

int checkMessages(int localSock, int networkSock)
{
    struct pollfd *fds = calloc(2 + clientsCnt, sizeof(struct pollfd));
    fds[0].fd = localSock;
    fds[0].events = POLLIN;
    fds[1].fd = networkSock;
    fds[1].events = POLLIN;

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < clientsCnt; ++i)
    {
        fds[i+2].fd = clients[i]->fd;
        fds[i+2].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);

    poll(fds, clientsCnt + 2, -1);
    int res = 0;
    for (int i = 0; i < clientsCnt + 2; ++i)
    {
        if (fds[i].revents & POLLIN)
        {
            res = fds[i].fd;
            break;
        }
    }
    if (res == localSock || res == networkSock)
        res = accept(res, NULL, NULL);
    free(fds);

    return res;
}

int getOpponent(int id)
{
    return id % 2 == 0 ? id + 1 : id - 1;
}

int addClient(char *name, int fd)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0)
            return -1;

    int id = -1;

    for (int i = 0; i < MAX_CLIENTS; i += 2)
    {
        if (clients[i] != NULL && clients[i+1] == NULL)
        {
            id = i + 1;
            break;
        }
    }

    if (id == -1)
    {
        for (int i = 0; i < MAX_CLIENTS; ++i)
        {
            if (clients[i] == NULL)
            {
                id = i;
                break;
            }
        }
    }

    if (id != -1)
    {
        struct Client *newClient = calloc(1, sizeof(struct Client));
        newClient->name = calloc(MAX_MSG_LEN, sizeof(char));
        strcpy(newClient->name, name);
        newClient->fd = fd;
        newClient->available = 1;

        clients[id] = newClient;
        clientsCnt++;
    }

    return id;
}

void freeClient(int id)
{
    int opponent = getOpponent(id);
    if (clients[opponent] != NULL)
    {
        printf("Removing %s's opponent - %s\n", clients[id]->name, 
               clients[opponent]->name);
        free(clients[opponent]->name);
        free(clients[opponent]);
        clients[opponent] = NULL;
        clientsCnt--;
    }

    free(clients[id]->name);
    free(clients[id]);
    clients[id] = NULL;
    clientsCnt--;
}

void removeClient(char *name)
{
    int id = -1;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0)
            id = i;

    printf("Removing client %s\n", name);
    freeClient(id);
}

void removeUnavailableClients()
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i] && clients[i]->available == 0)
        {
            printf("%s is unavailable\n", clients[i]->name);
            removeClient(clients[i]->name);
        }
    }
}

int findClient(char *name)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0)
            return i;
    return -1;
}

void sendPings()
{
    printf("Sending pings\n");
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i])
        {
            send(clients[i]->fd, "ping| ", MAX_MSG_LEN, 0);
            clients[i]->available = 0;
        }
    }
}

void* pingRoutine()
{
    for (;;)
    {
        pthread_mutex_lock(&mutex);
        removeUnavailableClients();
        sendPings();
        pthread_mutex_unlock(&mutex);
        sleep(5);
    }
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        printf("[ERROR] Required parameters: port, sockPath\n");
        exit(-1);
    }
    srand(time(NULL));

    char *port = argv[1];
    char *sockPath = argv[2];

    int localSock = setLocalSocket(sockPath);
    int networkSock = setNetworkSocket(port);

    pthread_t t;
    pthread_create(&t, NULL, &pingRoutine, NULL);

    char *command, *arg, *name;
    for (;;)
    {
        int clientFd = checkMessages(localSock, networkSock);
        char buffer[MAX_MSG_LEN+1];
        recv(clientFd, buffer, MAX_MSG_LEN, 0);

        printf("Received: '%s'\n", buffer);

        command = strtok(buffer, "|");
        arg = strtok(NULL, "|");
        name = strtok(NULL, "|");

        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0)
        {
            int id = addClient(name, clientFd);

            if (id == -1)
            {
                send(clientFd, "add|name_exists", MAX_MSG_LEN, 0);
                close(clientFd);
            }
            else if (id % 2 == 0)
                send(clientFd, "add|no_opponent", MAX_MSG_LEN, 0);
            else
            {
                int randVal = rand() % 100;
                int first, second;

                first = randVal % 2 == 0 ? id : getOpponent(id);
                second = randVal % 2 == 0 ? getOpponent(id) : id;

                send(clients[first]->fd, "add|O", MAX_MSG_LEN, 0);
                send(clients[second]->fd, "add|X", MAX_MSG_LEN, 0);
            }
        }

        if (strcmp(command, "move") == 0)
        {
            int move = atoi(arg);
            int player = findClient(name);
            sprintf(buffer, "move|%d", move);
            
            send(clients[getOpponent(player)]->fd, buffer, MAX_MSG_LEN,
                 0);
        }

        if (strcmp(command, "quit") == 0)
            removeClient(name);

        if (strcmp(command, "pong") == 0)
        {
            int player = findClient(name);
            if (player != -1)
                clients[player]->available = 1;
        }
        pthread_mutex_unlock(&mutex);
    }
}
