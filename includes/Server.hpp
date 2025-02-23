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
#define MAX_BUFFER 1024 * 3
#define MAX_CLIENTS 1024 * 2
#define PORT 1111
class Response;
class RequestParse;

class Server
{
private:
    enum EventsEnum {
        ADD_READ,
        ADD_WRITE,
        REMOVE_READ,
        REMOVE_WRITE,
    };
    struct eventClient
    {
        int client;
        struct kevent eventWrite;
        struct kevent eventRead;
    };
    int serverSocket;
    int kq;
    struct kevent event;
    std::map<int, RequestParse* > clientsRequest;
    std::map<int, Response* > clientsResponse;
    std::map<int, Conserver*> serversConfigs;
    std::map<int, int > serversClients;
    std::vector<int> servers;
    struct kevent timerEvent;
    static const int TIMEOUT_SECONDS = 2;
    void            ConnectWithClient(int server);
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
    Server();
    ~Server();
    int CreateServer(std::vector<Conserver> &config);
};