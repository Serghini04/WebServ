/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mal-mora <mal-mora@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 19:54:16 by mal-mora          #+#    #+#             */
/*   Updated: 2025/01/09 20:54:43 by mal-mora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server()
{
    isInterError = false;
}

Server::~Server()
{
    for (std::map<int, RequestParse *>::const_iterator
             it = clientsRequest.begin();
         it != clientsRequest.end(); it++)
    {
        delete it->second;
    }
}

void errorMsg(std::string str, int fd)
{
    std::cout << str << std::endl;
    close(fd);
}

void Server::SendError(int fd, std::string msg)
{
    std::cout << msg << std::endl;
    exit(0);
    this->clientsRequest[fd]->SetStatusCode(eInternalServerError);
    isInterError = true;
    SendData(fd);
}

int make_socket_nonblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) == -1)
    {
        perror("fcntl");
        return -1;
    }
    return 0;
}
void Server::ResponseEnds(int clientSocket)
{
    EV_SET(&event, clientSocket, EVFILT_WRITE, EV_CLEAR, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
        SendError(clientSocket, "kevent");
    EV_SET(&event, clientSocket, EVFILT_READ, EV_CLEAR, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
        SendError(clientSocket, "kevent");
    clientsResponse.erase(clientSocket);
    clientsRequest.erase(clientSocket);
    isInterError = false;
    close(clientSocket);
}

int Server::ConfigTheSocket()
{
    sockaddr_in addressSocket;
    int opt = 1;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
        return -1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt");
        return -1;
    }
    addressSocket.sin_addr.s_addr = INADDR_ANY;
    addressSocket.sin_family = AF_INET;
    addressSocket.sin_port = htons(PORT);

    if (make_socket_nonblocking(serverSocket) == -1)
        return 1;
    if (bind(serverSocket, (const sockaddr *)&addressSocket, sizeof(addressSocket)) == -1)
        return -1;
    if (listen(serverSocket, 5) == -1)
        return -1;
    return serverSocket;
}

void Server::RecivData(int clientSocket)
{
    int bytesRead;
    std::string fullData;
    static int isHeader = 1;
    char buffer[MAX_BUFFER];

    memset(buffer, 0, sizeof(buffer));
    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        if(errno == ECONNRESET)
        {
            ResponseEnds(clientSocket);
            return ;
        }
        return;
    }
    fullData.assign(buffer, bytesRead);
    (*clientsRequest[clientSocket]).readBuffer(fullData, isHeader);
    std::cout.flush();
    if ((*clientsRequest[clientSocket]).requestIsDone()) //false
    {
        puts("Data Recived");
        isHeader = 1;
        (*clientsRequest[clientSocket]).SetRequestIsDone(false);
        EV_SET(&event, clientSocket, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
            SendError(clientSocket, "kevent");
        return;
    }
}

void Server::SendData(int clientSocket)
{
    size_t totalSent = 0;
    size_t responseSize;
    std::ofstream test("outt.txt");

    std::string response = clientsResponse[clientSocket]->getResponse(*clientsRequest[clientSocket], isInterError);
    test << response;
    test.flush();
    exit(0);
    if (response.empty())
    {
        ResponseEnds(clientSocket);
        return;
    }
    responseSize = response.size();
    while (totalSent < responseSize)
    {
        ssize_t bytesSent = send(clientSocket,
                                 response.c_str() + totalSent,
                                 responseSize - totalSent,
                                 0);
        if (bytesSent <= 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            if(errno == EPIPE)
            {
                ResponseEnds(clientSocket);
                return ;
            }
            perror("Send failed");
            SendError(clientSocket, "SENDING Error");
            return;
        }
        totalSent += bytesSent;
    }
}

void Server::ConnectWithClient(int kq)
{
    sockaddr_in clientAddress;
    socklen_t clientAddrLen;
    int clientSocket;
    puts("new connection");
    clientAddrLen = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);
    if (clientSocket == -1)
        errorMsg("Cannot Connect", serverSocket);
    if (make_socket_nonblocking(clientSocket) == -1)
        SendError(clientSocket, "NonBloacking");
    else
    {
        EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
            SendError(clientSocket, "kevent5");
    }
}

void Server::HandelEvents(int n, struct kevent events[])
{
    int clientSocket;

    for (int i = 0; i < n; i++)
    {
        if (events[i].ident == static_cast<uintptr_t>(serverSocket))
            ConnectWithClient(kq);
        else if (events[i].filter == EVFILT_READ)
        {
            clientSocket = events[i].ident;
            if (clientsRequest.find(clientSocket) == clientsRequest.end())
                    clientsRequest[clientSocket] = new RequestParse();
            RecivData(clientSocket);
        }
        else if (events[i].filter == EVFILT_WRITE)
        {
            clientSocket = events[i].ident;
            if (clientsResponse.find(clientSocket) == clientsResponse.end())
                clientsResponse[clientSocket] = new Response();
            SendData(clientSocket);
        }
    }
}
int Server::CreateServer()
{
    serverSocket = ConfigTheSocket();
    struct kevent events[MAX_CLIENTS];

    if (serverSocket == -1)
        errorMsg("Socket Creation Fails", serverSocket);
    kq = kqueue();
    if (kq == -1)
        errorMsg("kqueue creation failed", serverSocket);
    EV_SET(&event, serverSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
        errorMsg("kqueue registration failed", serverSocket);
    while (true)
    {
        int n = kevent(kq, NULL, 0, events, MAX_CLIENTS, NULL);
        if (n == -1)
            errorMsg("kevent Fails", serverSocket);
        HandelEvents(n, events);
    }
    close(serverSocket);
    close(kq);
    return 0;
}
