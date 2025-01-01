#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/event.h>
#define MAX_BUFFER 4096
#define MAX_CLIENTS 128
#define PORT 3978



class Server
{
private:
    int serverSocket;
    int clientSocket;
    int kq;
    struct kevent event;
    std::vector<int> clients;

    void    connectWithClient(int serverEpoll);
    void    handelEvents(int n, struct kevent events[]);
    int     prepareTheSocket();
    void    SendData();
    void    RecivData();
public:
    Server();
    int CreateServer();
};


