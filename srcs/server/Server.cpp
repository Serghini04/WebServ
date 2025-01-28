/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meserghi <meserghi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 19:54:16 by mal-mora          #+#    #+#             */
/*   Updated: 2025/01/22 16:21:59 by meserghi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server()
{
}

Server::~Server()
{
    for (std::map<int, Conserver *>::const_iterator
             it = serversConfigs.begin();
         it != serversConfigs.end(); it++)
    {
        // delete it->second;
    }
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
    EV_SET(&event, clientSocket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
        SendError(clientSocket);
    EV_SET(&event, clientSocket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
        SendError(clientSocket);
    delete clientsRequest[clientSocket];
    delete clientsResponse[clientSocket];
    clientsResponse.erase(clientSocket);
    clientsRequest.erase(clientSocket);
}

void Server::ConnectionClosed(int clientSocket)
{
    CleanUpAllocation(clientSocket);
    close(clientSocket);
}
void Server::ResponseEnds(int clientSocket)
{
    if (clientsRequest[clientSocket]->isConnectionClosed())
        ConnectionClosed(clientSocket);
    else
        CleanUpAllocation(clientSocket);
}
int Server::ConfigTheSocket(Conserver &config)
{
    sockaddr_in addressSocket;
    int serverSocket;
    int opt = 1;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
        return -1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt SO_REUSEADDR failed");
        return -1;
    }
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt SO_REUSEPORT failed");
        return -1;
    }
    addressSocket.sin_addr.s_addr = INADDR_ANY;
    addressSocket.sin_family = AF_INET;
    // addressSocket.sin_port = htons(Utility::StrToInt(config.getAttributes("port")));
    addressSocket.sin_port = htons(PORT);
    if (make_socket_nonblocking(serverSocket) == -1)
        return -1;
    if (bind(serverSocket, (const sockaddr *)&addressSocket, sizeof(addressSocket)) == -1)
        return -1;
    if (listen(serverSocket, 1024) == -1)
        return -1;
    this->serversConfigs[serverSocket] = &config;
    return serverSocket;
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
        EV_SET(&event, clientSocket, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &event, 1, NULL, 0, NULL))
            SendError(clientSocket);
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

void Server::ConnectWithClient(int kq, uintptr_t server)
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
        serversClients[clientSocket] = server;
        EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
            SendError(clientSocket);
    }
}

void Server::HandelEvents(int n, struct kevent events[])
{
    int clientSocket;

    for (int i = 0; i < n; i++)
    {
        if (std::find(servers.begin(), servers.end(), (intptr_t)events[i].ident) != servers.end())
            ConnectWithClient(kq, events[i].ident);
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
    {
        serverSocket = ConfigTheSocket(config[i]);
        if (serverSocket == -1)
            errorMsg("Socket Creation Fails", serverSocket);
        EV_SET(&event, serverSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
            errorMsg("kqueue registration failed", serverSocket);
        servers.push_back(serverSocket);
    }
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