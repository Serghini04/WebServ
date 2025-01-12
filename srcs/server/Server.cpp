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
    
}

void errorMsg(std::string str, int fd)
{
    std::cerr << str << std::endl;
    close(fd);
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

void Server::RecivData(int clientSocket, RequestParse *request)
{
    int bytesRead;
    std::string fullData;
    static int isHeader = 1;
    std::ofstream mfile("testdd");
    char buffer[MAX_BUFFER];

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
        (*request).readBuffer(fullData, isHeader);
        if ((*request).requestIsDone())
        {
            puts("Data Recived");
            isHeader = 1;
            EV_SET(&event, clientSocket, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
            if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
                errorMsg("kevent Error", clientSocket);
            return;
        }
    }
}
void Server::SendData(int clientSocket, RequestParse *request)
{
    Response responseObj;
    std::ofstream mfile("test");
    // std::cout << "url : "<< (*request)._url << std::endl;
    while (true)   
    {
        std::string response = responseObj.getResponse(*request);
        mfile << response;
        mfile.flush();
        if (response.empty())
            break; 
        size_t totalSent = 0; 
        size_t responseSize = response.size();
        while (totalSent < responseSize)
        {
            ssize_t bytesSent = send(
                clientSocket,
                response.c_str() + totalSent, 
                responseSize - totalSent,    
                0);
            if (bytesSent <= 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    continue;
                perror("Send Error");
                close(clientSocket);
                return;
            }
            totalSent += bytesSent; 
        }
    }
    EV_SET(&event, clientSocket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
        perror("kevent Error: Removing EVFILT_WRITE");
    puts("Data Sent Succussfully");
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
        std::cout << "good" << std::endl;
        if (events[i].ident == static_cast<uintptr_t>(serverSocket))
            connectWithClient(kq);
        else if(events[i].filter == EVFILT_READ)
        {
            clientSocket = events[i].ident;
            clientsRequests[clientSocket] = std::unique_ptr<RequestParse>(new RequestParse());
            RecivData(clientSocket, clientsRequests[clientSocket].get());
        }
        else if(events[i].filter == EVFILT_WRITE)
        {
            clientSocket = events[i].ident;
            SendData(clientSocket, clientsRequests[clientSocket].get());
            close(clientSocket);
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
