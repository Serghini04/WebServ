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
#include <csignal>
#include <map>
#include <Response.hpp>
#include <RequestParse.hpp>
#define MAX_BUFFER 40960
#define MAX_CLIENTS 128
#define PORT 3938

class Server
{
private:
    int serverSocket;
    int kq;
    struct kevent event;
    std::vector<int> clients;

    void            connectWithClient(int serverEpoll);
    void            handelEvents(int n, struct kevent events[], RequestParse &req);
    int             prepareTheSocket();
    void            SendData(int clientSocket, RequestParse &request);
    void            RecivData(int clientSocket, RequestParse &request);
public:
    Server();
    int CreateServer();
};

