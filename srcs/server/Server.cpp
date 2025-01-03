/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mal-mora <mal-mora@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/02 19:54:16 by mal-mora          #+#    #+#             */
/*   Updated: 2025/01/03 17:36:28 by mal-mora         ###   ########.fr       */
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
void Server::RecivData(int clientSocket)
{
    char buffer[MAX_BUFFER] = {0};
    int bytesRead;
    RequestParse request;
    std::string fullData;
    while (true)
    {
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
            return;
        }
        else
            fullData.append(buffer, bytesRead);
    }
    request.readBuffer(fullData);
    SendData(clientSocket);
}
void Server::SendData(int clientSocket)
{
    size_t totalSent = 0;
    size_t dataSize;

    std::string responseBody = Response::FileToString("index.html");
    std::ostringstream oss;
    oss << responseBody.size();
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " +
        oss.str() + "\r\n"
                    "\r\n" +
        responseBody;
    dataSize = response.size();
    while (totalSent < dataSize)
    {
        ssize_t bytesSent = send(clientSocket, response.c_str() + totalSent, dataSize - totalSent, 0);
        if (bytesSent <= 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            perror("Send Error");
            break;
        }
        totalSent += bytesSent;
    }
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
void Server::handelEvents(int n, struct kevent events[])
{
    int clientSocket;

    for (int i = 0; i < n; i++)
    {
        if (events[i].ident == static_cast<uintptr_t>(serverSocket))
            connectWithClient(kq);
        else
        {
            clientSocket = events[i].ident;
            RecivData(clientSocket);
        }
    }
}
int Server::CreateServer()
{
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
        handelEvents(n, events);
    }
    close(serverSocket);
    close(kq);
    return 0;
}
