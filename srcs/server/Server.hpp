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

class Server
{
private:
    int serverSocket;
    epoll_event event;
    epoll_event eventResults[10];
    int clientSocket;
    void connectWithClient(int serverEpoll);
    int     prepareTheSocket();
    int     SendData(int clientSocket);
public:
    Server();
    int CreateServer();
};


