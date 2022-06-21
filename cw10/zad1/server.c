#include "common.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct Client *clients[MAX_CLIENTS] = {NULL};
int clientsCnt = 0;

int getOpponent(int idx)
{
    return idx % 2 == 0 ? idx + 1 : idx - 1;
}


int addClient(char *name, int fd)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0)
            return -1;

    int idx = -1;

    // another waiting struct Client
    for (int i = 0; i < MAX_CLIENTS; i += 2)
    {
        if (clients[i] != NULL && clients[i+1] == NULL)
        {
            idx = i + 1;
            break;
        }
    }

    // no opponent - first free place
    if (idx == -1)
    {
        for (int i = 0; i < MAX_CLIENTS; ++i)
        {
            if (clients[i] == NULL)
            {
                idx = i;
                break;
            }
        }
    }

    if (idx != -1)
    {
        struct Client *newClient = calloc(1, sizeof(struct Client));
        newClient->name = calloc(MAX_MSG_LEN, sizeof(char));
        strcpy(newClient->name, name);
        newClient->fd = fd;
        newClient->available = true;

        clients[idx] = newClient;
        clientsCnt++;
    }

    return idx;
}


int findClient(char *name)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0)
            return i;
    return -1;
}

void freeClient(int index)
{
    free(clients[index]->name);
    free(clients[index]);
    clients[index] = NULL;
    clientsCnt--;

    int opponent = getOpponent(index);

    // remove opponent if exists
    if (clients[getOpponent(index)] != NULL)
    {
        printf("Removing opponent: %s\n", clients[opponent]->name);
        free(clients[opponent]->name);
        free(clients[opponent]);
        clients[opponent] = NULL;
        clientsCnt--;
    }
}

void removeClient(char *name)
{
    int idx = -1;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0)
            idx = i;

    printf("Removing client: %s\n", name);
    freeClient(idx);
}


void removeUnavailableClients()
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i] && ! clients[i]->available)
        {
            printf("not available\n");
            removeClient(clients[i]->name);
        }
    }
}

void sendPings()
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i])
        {
            send(clients[i]->fd, "ping: ", MAX_MSG_LEN, 0);
            clients[i]->available = false;
        }
    }
}

void* ping()
{
    while (true)
    {
        printf("PING\n");
        pthread_mutex_lock(&mutex);
        removeUnavailableClients();
        sendPings();
        pthread_mutex_unlock(&mutex);

        sleep(10);
    }
}

int setLocalSocket(char* sockPath)
{
    // create socket
    int sockFd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sockFd  == -1)
    {
        printf("Could not create LOCAL socket\n");
        exit(1);
    }
    else
        printf("LOCAL socket created\n");

    struct sockaddr_un sockAddr;
    memset(&sockAddr, 0, sizeof(struct sockaddr_un));
    sockAddr.sun_family = AF_UNIX;
    strcpy(sockAddr.sun_path, sockPath);

    // unlink the file so the bind will succeed
    unlink(sockPath);

    // assigns the address to the socket
    if ((bind(sockFd, (struct sockaddr *) &sockAddr, sizeof(sockAddr))) == -1)
    {
        printf("Could not bind LOCAL socket\n");
        exit(1);
    }

    // listen for connections on a socket
    if ((listen(sockFd, MAX_CLIENTS)) == -1)
    {
        printf("Listen on LOCAL socket failed\n");
        exit(1);
    }

    printf("LOCAL socket fd: %d\n", sockFd);

    return sockFd;
}

int setNetworkSocket(char *port)
{
    // create socket
    int sockFd = socket(AF_INET , SOCK_STREAM, 0);

    if (sockFd  == -1)
    {
        printf("Could not create INET socket\n");
        exit(1);
    }
    else
        printf("INET socket created\n");

    // set port and IP
    struct sockaddr_in sockAddr;
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(atoi(port));

    // assigns the address to the socket
    if ((bind(sockFd, (struct sockaddr *) &sockAddr, sizeof(sockAddr))) == -1)
    {
        printf("Could not bind INET socket\n");
        exit(1);
    }

    // listen_for_msg for connections on a socket
    if ((listen(sockFd, MAX_CLIENTS)) == -1)
    {
        printf("Listen on INET socket failed\n");
        exit(1);
    }

    printf("INET socket fd: %d\n", sockFd);

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
    int retval;

    for (int i = 0; i < clientsCnt + 2; ++i)
    {
        if (fds[i].revents & POLLIN)
        {
            retval = fds[i].fd;
            break;
        }
    }
    if (retval == localSock || retval == networkSock)
        retval = accept(retval, NULL, NULL);
    free(fds);

    return retval;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Wrong number of arguments!\n");
        exit(1);
    }
    srand(time(NULL));

    char *port = argv[1];
    char *sockPath = argv[2];

    int localSock = setLocalSocket(sockPath);
    int networkSock = setNetworkSocket(port);

    pthread_t t;
    pthread_create(&t, NULL, &ping, NULL);
    while (true)
    {
        int clientFd = checkMessages(localSock, networkSock);
        char buffer[MAX_MSG_LEN+1];
        recv(clientFd, buffer, MAX_MSG_LEN, 0);

        printf("%s\n", buffer);
        char *command = strtok(buffer, ":");
        char *arg = strtok(NULL, ":");
        char *name = strtok(NULL, ":");

        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0)
        {
            int index = addClient(name, clientFd);

            if (index == -1)
            {
                send(clientFd, "add:name_taken", MAX_MSG_LEN, 0);
                close(clientFd);
            }

            else if (index % 2 == 0)
                send(clientFd, "add:no_enemy", MAX_MSG_LEN, 0);

            else
            {
                int randVal = rand() % 100;
                int first, second;

                if (randVal % 2 == 0)
                {
                    first = index;
                    second = getOpponent(index);
                }
                else
                {
                    second = index;
                    first = getOpponent(index);
                }

                send(clients[first]->fd, "add:O",
                     MAX_MSG_LEN, 0);
                send(clients[second]->fd, "add:X",
                     MAX_MSG_LEN, 0);
            }
        }

        if (strcmp(command, "move") == 0)
        {
            int move = atoi(arg);
            int player = findClient(name);

            sprintf(buffer, "move:%d", move);
            send(clients[getOpponent(player)]->fd, buffer, MAX_MSG_LEN,
                 0);
        }
        if (strcmp(command, "end") == 0)
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
