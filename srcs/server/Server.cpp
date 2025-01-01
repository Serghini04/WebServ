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
void Server::RecivData()
{
    char buffer[MAX_BUFFER] = {0};
    int bytesRead;

    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << buffer << std::endl;
    if (bytesRead == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            std::cout << "No data available for now (non-blocking).\n";
            return;
        }
        perror("Read Error");
    }
    else if (bytesRead == 0)
        close(clientSocket);
    else
    {
        EV_SET(&event, clientSocket, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
            errorMsg("kqueue registration failed", clientSocket);
    }
}
void Server::SendData()
{
    std::string responseBody = "Salam w3alikom ";
    std::ostringstream oss;
    oss << responseBody.size();
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " +
        oss.str() + "\r\n"
                    "\r\n" +
        responseBody;
    if (int n = send(clientSocket, response.c_str(), response.size(), 0) == -1)
        perror("Send Error");
}

void Server::connectWithClient(int kq)
{
    sockaddr_in clientAddress;
    socklen_t clientAddrLen;

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
    for (int i = 0; i < n; i++)
    {
        if (events[i].ident == static_cast<uintptr_t>(serverSocket))
            connectWithClient(kq);
        else if (events[i].filter == EVFILT_READ)
        {
            clientSocket = events[i].ident;
            RecivData();
        }
        else if (events[i].filter == EVFILT_WRITE)
        {
            clientSocket = events[i].ident;
            SendData();
        }else
            perror("Event Error");
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
        errorMsg("kqueue registration failed", clientSocket);
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
