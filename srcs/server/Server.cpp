/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mal-mora <mal-mora@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 19:54:16 by mal-mora          #+#    #+#             */
/*   Updated: 2025/01/07 14:49:25 by mal-mora         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server()
{
    
}

void errorMsg(std::string str, int fd)
{
    std::cerr << str << std::endl;
    close(fd);
    // exit(1);
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

int Server::prepareTheSocket()
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

void Server::RecivData(int clientSocket, RequestParse &request)
{
    int bytesRead;
    std::string fullData;
    static int isHeader = 1;
    
    char buffer[1024 * 5];

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            perror("Read Error");
            break;
        }
        else if (bytesRead == 0)
        {
            close(clientSocket);
            break;
        }
        fullData.assign(buffer, bytesRead);
        request.readBuffer(fullData, isHeader);
        if (request.requestIsDone())
        {
            isHeader = 1;
            EV_SET(&event, clientSocket, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
            if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
                errorMsg("kevent Error", clientSocket);
            return;
        }
    }
}

void Server::SendData(int clientSocket, RequestParse &request)
{
    size_t totalSent = 0;
    size_t dataSize;
    std::string response;
    Response responseObj;

    response = responseObj.getResponse(request);
    dataSize = response.size();
    std::cout << dataSize << std::endl;
    exit(0);
    while (totalSent < dataSize)
    {
        ssize_t bytesSent = send(clientSocket, response.c_str() + totalSent, dataSize - totalSent, 0);
        if (bytesSent <= 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            perror("Send Error");
            break;
        }
        totalSent += bytesSent;
    }
    close(clientSocket);
}


void Server::connectWithClient(int kq)
{
    sockaddr_in clientAddress;
    socklen_t clientAddrLen;
    int clientSocket;

    clientAddrLen = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);
    if (clientSocket == -1)
        errorMsg("Accept Error", clientSocket);
    if (make_socket_nonblocking(clientSocket) == -1)
        errorMsg("Non-Blocking Error", clientSocket);
    else
    {
        EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
            errorMsg("kevent Error", clientSocket);
    }
}
void Server::handelEvents(int n, struct kevent events[], RequestParse &req)
{
    int clientSocket;

    for (int i = 0; i < n; i++)
    {
        if (events[i].ident == static_cast<uintptr_t>(serverSocket))
            connectWithClient(kq);
        else if(events[i].filter == EVFILT_READ)
        {
            clientSocket = events[i].ident;
            RecivData(clientSocket, req);
        }
        else if(events[i].filter == EVFILT_WRITE)
        {
            clientSocket = events[i].ident;
            SendData(clientSocket, req);
        }
    }
}
int Server::CreateServer()
{
    RequestParse req;
    serverSocket = prepareTheSocket();
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
        handelEvents(n, events, req);
    }
    close(serverSocket);
    close(kq);
    return 0;
}
