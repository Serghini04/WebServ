/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 19:54:16 by mal-mora          #+#    #+#             */
/*   Updated: 2025/02/26 10:32:10 by hidriouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
Server::Server()
{
}
void Server::setupConnectionTimer(int clientSocket)
{
    EV_SET(&timerEvent,
           clientSocket,
           EVFILT_TIMER,
           EV_ADD | EV_ONESHOT,
           NOTE_SECONDS,
           TIMEOUT_SECONDS,
           NULL);
    if (kevent(kq, &timerEvent, 1, NULL, 0, NULL) == -1)
    {
        SendError(clientSocket);
        return;
    }
}
Server::~Server()
{
    for (std::unordered_map<int, Conserver *>::const_iterator
             it = serversConfigs.begin();
         it != serversConfigs.end(); it++)
    {
        delete it->second;
    }
}

void Server::manageEvents(enum EventsEnum events, int clientSocket)
{
    switch (events)
    {
    case ADD_READ:
        EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
        break;
    case ADD_WRITE:
        EV_SET(&event, clientSocket, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        break;
    case REMOVE_READ:
        EV_SET(&event, clientSocket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        break;
    case REMOVE_WRITE:
        EV_SET(&event, clientSocket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        break;
    }
    kevent(kq, &event, 1, NULL, 0, NULL);
}
void errorMsg(std::string str, int fd)
{
    std::cout << str << strerror(errno) << std::endl;
    close(fd);
}

void Server::SendError(int fd)
{
    std::cout << "Internal Error" << std::endl;
    this->clientsRequest[fd]->SetStatusCode(eInternalServerError);
    this->clientsRequest[fd]->SetStatusCodeMsg("500 Internal Error");
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
void Server::CleanUpAllocation(int clientSocket)
{
    if (clientsRequest.count(clientSocket))
    {
        manageEvents(REMOVE_READ, clientSocket);
        delete clientsRequest[clientSocket];
        clientsRequest.erase(clientSocket);
    }
    if (clientsResponse.count(clientSocket))
    {
        manageEvents(REMOVE_WRITE, clientSocket);
        delete clientsResponse[clientSocket];
        clientsResponse.erase(clientSocket);
    }
}

void Server::ConnectionClosed(int clientSocket)
{
    puts("Connection Closed");
    CleanUpAllocation(clientSocket);
    // close(clientSocket);
}

void Server::ResponseEnds(int clientSocket)
{
    if (clientsRequest[clientSocket])
    {
        puts("Response Ends ");
        if (clientsRequest[clientSocket]->isCGI())
            clientsRequest[clientSocket]->setIsCGI(false);
        if (clientsRequest[clientSocket]->isConnectionClosed())
            ConnectionClosed(clientSocket);
        else
            CleanUpAllocation(clientSocket);
    }
}
#include <netdb.h>
#include <arpa/inet.h>
void Server::ConfigTheSocket(Conserver &config)
{
    int serverSocket;
    int opt = 1;
    std::vector<std::pair<std::string, std::string> > address = config.getlistening();
    for (size_t i = 0; i < address.size(); i++)
    {
        sockaddr_in addressSocket;

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1)
            errorMsg("Socket Creation Fails", serverSocket);
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
            errorMsg("nonblocking Fails", serverSocket);
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
            errorMsg("setsockopt Fails", serverSocket);
        if (make_socket_nonblocking(serverSocket) == -1)
            errorMsg("nonblocking Fails", serverSocket);
        addressSocket.sin_family = AF_INET;
        addressSocket.sin_port = htons(Utility::StrToInt(address[i].second.c_str()));
        addressSocket.sin_addr.s_addr = inet_addr(address[i].first.c_str());
        int bindResult = bind(serverSocket, (const sockaddr *)&addressSocket, sizeof(addressSocket));
        if (bindResult == -1)
            errorMsg("bind Fails", serverSocket);
        if (listen(serverSocket, 128) == -1)
            errorMsg("listen Fails", serverSocket);
        this->serversConfigs[serverSocket] = &config;
        if (serverSocket == -1)
            errorMsg("Socket Creation Fails", serverSocket);
        EV_SET(&event, serverSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
            errorMsg("kqueue registration failed", serverSocket);
        servers.push_back(serverSocket);
    }
}

void Server::RecivData(int clientSocket)
{
    int bytesRead;
    std::string fullData;
    char buffer[MAX_BUFFER];

    memset(buffer, 0, sizeof(buffer));
    if (!clientsRequest[clientSocket])
        return;
    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        if (errno == ECONNRESET || bytesRead == 0)
        {
            ConnectionClosed(clientSocket);
            return;
        }
        return;
    }
    fullData.assign(buffer, bytesRead);
    (*clientsRequest[clientSocket]).readBuffer(fullData);

    if ((*clientsRequest[clientSocket]).requestIsDone())
    {
        puts("Recive Data");
        clientsRequest[clientSocket]->SetRequestIsDone(false);
        manageEvents(ADD_WRITE, clientSocket);
        if ((*clientsRequest[clientSocket]).isCGI())
        {
            (*clientsRequest[clientSocket]).runcgiscripte();
        }
        return;
    }
}
void Server::SendData(int clientSocket)
{
    long long totalSent = 0;
    long long responseSize;

    if (!clientsResponse.count(clientSocket))
        clientsResponse[clientSocket] = new Response(*serversConfigs[serversClients[clientSocket]],
                                                     clientsRequest[clientSocket]);
    std::string response = clientsResponse[clientSocket]->getResponse();
    responseSize = response.size();
    if (response.empty())
    {
        ResponseEnds(clientSocket);
        return;
    }
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
            if (errno == EPIPE || bytesSent == 0)
            {
                ResponseEnds(clientSocket);
                return;
            }
            SendError(clientSocket);
            return;
        }
        totalSent += bytesSent;
    }
}

void Server::ConnectWithClient(uintptr_t server)
{
    sockaddr_in clientAddress;
    socklen_t clientAddrLen;
    int clientSocket;

    clientAddrLen = sizeof(clientAddress);
    clientSocket = accept(server, (struct sockaddr *)&clientAddress, &clientAddrLen);
    if (clientSocket == -1)
        errorMsg("Cannot Connect", server);
    puts("new connection ");
    if (make_socket_nonblocking(clientSocket) == -1)
        SendError(clientSocket);
    else
    {
        serversClients[clientSocket] = (int)server;
        setupConnectionTimer(clientSocket);
        manageEvents(ADD_READ, clientSocket);
    }
}

void Server::HandelEvents(int n, struct kevent events[])
{
    int clientSocket;
    for (int i = 0; i < n; i++)
    {
        if (std::find(servers.begin(), servers.end(), (intptr_t)events[i].ident) != servers.end())
            ConnectWithClient(events[i].ident);
        else if (events[i].filter == EVFILT_READ)
        {
            clientSocket = events[i].ident;
            if (!clientsRequest.count(clientSocket))
            {
                
                clientsRequest[clientSocket] = new RequestParse(
                    *serversConfigs[serversClients[clientSocket]]);
            }
            RecivData(clientSocket);
        }
        else if (events[i].filter == EVFILT_WRITE)
        {
            clientSocket = events[i].ident;
            SendData(clientSocket);
        }
        else if (events[i].filter == EVFILT_TIMER)
        {
            clientSocket = events[i].ident;
            if (!clientsRequest.count(clientSocket))
                close(clientSocket);
        }
    }
}

int Server::CreateServer(std::vector<Conserver> &config)
{
    int serverSocket = 0;
    struct kevent events[MAX_CLIENTS];

    kq = kqueue();
    if (kq == -1)
        errorMsg("kqueue creation failed", serverSocket);
    for (size_t i = 0; i < config.size(); i++)
        ConfigTheSocket(config[i]);
    while (true)
    {
        int n = kevent(kq, NULL, 0, events, MAX_CLIENTS, NULL);
        if (n == -1)
            errorMsg("kevent Fails", serverSocket);
        HandelEvents(n, events);
    }
    close(kq);
    return 0;
}
