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
#include <netinet/tcp.h>  
#include <unordered_map>
#define MAX_BUFFER 1024 * 2
#define MAX_CLIENTS 1024 * 2
#define PORT 1111

class Response;

class Server
{
private:
    enum EventsEnum {
        ADD_READ,
        ADD_WRITE,
        REMOVE_READ,
        REMOVE_WRITE,
    };
    int kq;
    struct kevent event;
    std::unordered_map<int, RequestParse* > clientsRequest;
    std::unordered_map<int, Response* > clientsResponse;
    std::vector<int> servers;
    struct kevent timerEvent;
    static const int TIMEOUT_SECONDS = 20;
    void            ConnectWithClient(uintptr_t server);
    void            HandelEvents(int n, struct kevent events[]);
    void            ConfigTheSocket(Conserver &config);
    void            SendData(int clientSocket);
    void            RecivData(int clientSocket);
    void            ResponseEnds(int clientSocket);
    void            SendError(int fd);
    void            ConnectionClosed(int clientSocket);
    void            CleanUpAllocation(int clientSocket);
    void            setupConnectionTimer(int clientSocket);
    void            manageEvents(enum EventsEnum events,int clientSocket);
public:
    std::unordered_map<int, std::vector<Conserver*> > serversConfigs;
    std::unordered_map<int, int > serversClients;
    Server();
    ~Server();
    int CreateServer(std::vector<Conserver> &config);
};