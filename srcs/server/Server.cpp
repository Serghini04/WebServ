#include "../includes/Server.hpp"

Server::Server()
{

}

void errorMsg(std::string str, int fd)
{
    std::cerr << str << std::endl;
    close(fd);
}

int make_socket_nonblocking(int sockfd)
{
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl");
        return -1;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl");
        return -1;
    }
    return 0;
}

int Server::prepareTheSocket()
{
    sockaddr_in addressSocket;

    // AF_INET stands for IPV4, SOCK_STREAM means i create a TCP Socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
        return -1;
    addressSocket.sin_addr.s_addr = INADDR_ANY;
    addressSocket.sin_family = AF_INET;
    addressSocket.sin_port = htons(3000);

    if (bind(serverSocket, (const sockaddr *)&addressSocket, sizeof(addressSocket)) == -1)
        return -1;

    if(listen(serverSocket, 5) == -1)
        return -1;
    return serverSocket;
}



int Server::SendData(int clientSocket)
{
    char buffer[1024];
    int bytesRead;

    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    //buffer should be parsed and retuerned
    // 
    if (bytesRead == -1)
        perror("Read Error");
    else if (bytesRead == 0)
        close(clientSocket);
    else
    {
        std::string message(buffer);
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n Salam w3alikom " + message;
        send(clientSocket, response.c_str(), response.size(), 0);
    }
    return 0;
}

void Server::connectWithClient(int serverEpoll)
{
    sockaddr_in clientAddress;
    socklen_t clientAddrLen;
    
    clientAddrLen = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);
    if(clientSocket == -1)
        errorMsg("Accept Error", clientSocket);
    if(make_socket_nonblocking(clientSocket))
        errorMsg("Non-Blocking Error", clientSocket);
    else
    {
        event.events = EPOLLIN | EPOLLPRI;
        event.data.fd = clientSocket;
        if (epoll_ctl(serverEpoll, EPOLL_CTL_ADD, clientSocket, &event) == -1)
            errorMsg("epoll_ctl Error", serverSocket);
    }
   
}

int Server::CreateServer()
{
    int serverEpoll;

    serverSocket = prepareTheSocket();
    if (serverSocket == -1)
        errorMsg("Socket Creation Fails", serverSocket);
    if ((serverEpoll = epoll_create1(0)) == -1)
        errorMsg("Epoll Creation Fails", serverEpoll);

    event.data.fd = serverSocket;
    event.events = EPOLLIN;
    if (epoll_ctl(serverEpoll, EPOLL_CTL_ADD, serverSocket, &event))
        errorMsg("epoll_ctl Fails", serverSocket);

    while (true)
    {
        int n = epoll_wait(serverEpoll, eventResults, 10, -1);
        if (n == -1)
            errorMsg("epoll_wait Fails", serverSocket);
        for (int i = 0; i < n; i++)
        {
            if (eventResults[i].data.fd == serverSocket)
                connectWithClient(serverEpoll);
            else
            {
                clientSocket = eventResults[i].data.fd;
                SendData(clientSocket);
            }
        }
    }
    close(serverSocket);
    close(clientSocket);
    return 0;
}
