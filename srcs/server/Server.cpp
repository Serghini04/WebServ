#include <Server.hpp>
#include <RequestParse.hpp>

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

    // AF_INET stands for IPV4, SOCK_STREAM means i create a TCP Socket
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
    addressSocket.sin_port = htons(3789);

    if (make_socket_nonblocking(serverSocket) == -1)
        return 1;
    if (bind(serverSocket, (const sockaddr *)&addressSocket, sizeof(addressSocket)) == -1)
        return -1;
    if (listen(serverSocket, 5) == -1)
        return -1;
    return serverSocket;
}

int Server::SendData(int clientSocket)
{
    char buffer[1025];
    int bytesRead;
    RequestParse    request;

    bytesRead = recv(clientSocket, buffer, sizeof(buffer) + 1, 0);
    buffer[1024] = '\0';
    request.readBuffer(buffer);
    if (bytesRead == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            std::cout << "No data available for now (non-blocking).\n";
            return 0;
        }
        perror("Read Error");
    }
    else if (bytesRead == 0)
        close(clientSocket);
    else
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

        send(clientSocket, response.c_str(), response.size(), 0);
    }
    return 0;
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
        std::cout << "Connection occurs" << std::endl;
        EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        kevent(kq, &event, 1, NULL, 0, NULL);
    }
}


int Server::CreateServer()
{
    serverSocket = prepareTheSocket();
    if (serverSocket == -1)
        errorMsg("Socket Creation Fails", serverSocket);

    int kq = kqueue();
    if (kq == -1)
        errorMsg("kqueue creation failed", serverSocket);

    EV_SET(&event, serverSocket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
        errorMsg("kqueue registration failed", clientSocket);

    while (true)
    {
        struct kevent events[10];
        int n = kevent(kq, NULL, 0, events, 10, NULL);
        if (n == -1)
            errorMsg("epoll_wait Fails", serverSocket);
        for (int i = 0; i < n; i++)
        {
            if (events[i].ident == static_cast<uintptr_t>(serverSocket))
                connectWithClient(kq);
            else
            {
                clientSocket = events[i].ident;
                SendData(clientSocket);
            }
        }
    }
    close(serverSocket);
    close(kq);
    return 0;
}
