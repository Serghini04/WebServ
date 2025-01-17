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
#include "ConServer.hpp"
#define MAX_BUFFER 1024
#define MAX_CLIENTS 128
#define PORT 3938
class Response;
class RequestParse;
class Server
{
private:
    int serverSocket;
    int kq;
    struct kevent event;
    std::map<int, RequestParse* > clientsRequest;
    std::map<int, Response* > clientsResponse;
    std::map<int, Conserver* > serversConfigs;
    std::map<int, int > serversClients;
    std::vector<int> servers;
    void            ConnectWithClient(int kq, uintptr_t server);
    void            HandelEvents(int n, struct kevent events[]);
    int             ConfigTheSocket(Conserver &config);
    void            SendData(int clientSocket);
    void            RecivData(int clientSocket);
    void            ResponseEnds(int clientSocket);
    void            SendError(int fd);
public:
    Server();
    ~Server();
    int CreateServer(std::vector<Conserver> &config);
};

