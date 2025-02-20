/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hidriouc <hidriouc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 19:54:16 by mal-mora          #+#    #+#             */
/*   Updated: 2025/02/05 13:48:08 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server()
{
}
void Server::setupConnectionTimer(int clientSocket)
{
    // Set up a timer event for this connection
    EV_SET(&timerEvent,
           clientSocket,        // Use client socket as identifier
           EVFILT_TIMER,        // Timer filter
           EV_ADD | EV_ONESHOT, // One-shot timer
           NOTE_SECONDS,        // Use seconds resolution
           TIMEOUT_SECONDS,     // 5 second timeout
           NULL);               // No user data

    if (kevent(kq, &timerEvent, 1, NULL, 0, NULL) == -1)
    {
        SendError(clientSocket);
        return;
    }
}
Server::~Server()
{
    for (std::map<int, Conserver *>::const_iterator
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
        EV_SET(&event, clientSocket, EVFILT_READ, EV_CLEAR, 0, 0, NULL);
        break;
    case REMOVE_WRITE:
        EV_SET(&event, clientSocket, EVFILT_READ, EV_CLEAR, 0, 0, NULL);
        break;
    }
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
        SendError(clientSocket);
}
void errorMsg(std::string str, int fd)
{
    std::cout << str << strerror(errno) << std::endl;
    close(fd);
}

void Server::SendError(int fd)
{
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
    manageEvents(REMOVE_READ, clientSocket);
    manageEvents(REMOVE_WRITE, clientSocket);
    delete clientsRequest[clientSocket];
    delete clientsResponse[clientSocket];
    clientsResponse.erase(clientSocket);
    clientsRequest.erase(clientSocket);
}

void Server::ConnectionClosed(int clientSocket)
{
    if (clientsRequest.count(clientSocket))
        CleanUpAllocation(clientSocket);
    close(clientSocket);
}
void Server::ResponseEnds(int clientSocket)
{
    clientsRequest[clientSocket]->SetRequestIsDone(false);
    if(clientsRequest[clientSocket]->isCGI())
        clientsRequest[clientSocket]->setIsCGI(false);
    if (clientsRequest[clientSocket]->isConnectionClosed())
        ConnectionClosed(clientSocket);
    else
        CleanUpAllocation(clientSocket);
}

void Server::ConfigTheSocket(Conserver &config)
{
    int serverSocket;
    int opt = 1;
    std::vector<std::pair<std::string, std::string> > address = config.getlistening();
    for (size_t i = 0; i < address.size(); i++)
    {
        sockaddr_in addressSocket;

        memset(&addressSocket, 0, sizeof(addressSocket));
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
        addressSocket.sin_addr.s_addr = INADDR_ANY;
        // addressSocket.sin_addr.s_addr = inet_addr(address[i].first.c_str());
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
    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        if (errno == ECONNRESET)
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
        puts("Data Recived");
        manageEvents(REMOVE_READ, clientSocket);
        manageEvents(ADD_WRITE, clientSocket);
        // hidriouc add folowing line for test runing script ??
        if ((*clientsRequest[clientSocket]).isCGI())
            if((*clientsRequest[clientSocket]).runcgiscripte() != 200)
            {
                clientsRequest[clientSocket]->SetStatusCode(eNotFound);
                clientsRequest[clientSocket]->SetStatusCodeMsg("Not Found");
            }
        return;
    }
}

void Server::SendData(int clientSocket)
{
    size_t totalSent = 0;
    size_t responseSize;

    if (!clientsResponse.count(clientSocket))
        clientsResponse[clientSocket] = new Response(*serversConfigs[serversClients[clientSocket]],
                                                     *clientsRequest[clientSocket]);
    std::string response = clientsResponse[clientSocket]->getResponse();
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
                return;
            if (errno == EPIPE)
            {
                ConnectionClosed(clientSocket);
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

    puts("new connection");
    clientAddrLen = sizeof(clientAddress);
    clientSocket = accept(server, (struct sockaddr *)&clientAddress, &clientAddrLen);
    if (clientSocket == -1)
        errorMsg("Cannot Connect", serverSocket);
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
            if (clientsRequest.find(clientSocket) == clientsRequest.end())
                clientsRequest[clientSocket] = new RequestParse(*serversConfigs[serversClients[clientSocket]]);
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
            ConnectionClosed(clientSocket);
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