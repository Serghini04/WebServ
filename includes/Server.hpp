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
#include <Utility.hpp>
#include <Response.hpp>
#include <RequestParse.hpp>
#define MAX_BUFFER 4096
#define MAX_CLIENTS 128
#define PORT 3938

class Server
{
private:
    int serverSocket;
    int kq;
    struct kevent event;
    std::vector<int> clients;

    void    connectWithClient(int serverEpoll);
    void    handelEvents(int n, struct kevent events[]);
    int     prepareTheSocket();
    void    SendData(int clientSocket);
    void    RecivData(int clientSocket);
public:
    Server();
    int CreateServer();
};


