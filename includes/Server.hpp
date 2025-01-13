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
#define MAX_BUFFER 1024 * 5
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
    void            ConnectWithClient(int serverEpoll);
    void            HandelEvents(int n, struct kevent events[]);
    int             ConfigTheSocket();
    void            SendData(int clientSocket);
    void            RecivData(int clientSocket);
    void            ResponseEnds(int clientSocket);
    void            SendError(int fd, std::string msg);
    bool            isInterError;
public:
    Server();
    ~Server();
    int CreateServer();
};

