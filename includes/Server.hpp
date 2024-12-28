#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#ifdef __APPLE__ // macOS and BSD-based systems
#include <sys/event.h>
#endif
class Server
{
private:
    int serverSocket;
    epoll_event event;
    epoll_event eventResults[10];
    int clientSocket;
    // void connectWithClient(int serverEpoll);
    void    connectWithClient(int serverEpoll);
    int     prepareTheSocket();
    // int     SendData(int clientSocket);
    int     SendData(int clientSocket);
    std::vector<int> clients;
public:
    Server();
    int CreateServer();
};


