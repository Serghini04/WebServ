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
class Server
{
private:
    int serverSocket;
    int clientSocket;
    struct kevent event;
    void    connectWithClient(int serverEpoll);
    int     prepareTheSocket();
    int     SendData(int clientSocket);
    std::vector<int> clients;
public:
    Server();
    int CreateServer();
};


