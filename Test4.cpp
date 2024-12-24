#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstring>

#include <fstream>
#include <sstream>

#define MAX_EVENTS 10

// Non-blocking socket setup
int make_socket_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        return -1;
    }
    return 0;
}

int main() {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket");
        return 1;
    }

    // Make the socket non-blocking
    if (make_socket_nonblocking(serverSocket) == -1) {
        close(serverSocket);
        return 1;
    }

    // Specify server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8180);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("bind");
        close(serverSocket);
        return 1;
    }

    // Start listening
    if (listen(serverSocket, 5) == -1) {
        perror("listen");
        close(serverSocket);
        return 1;
    }

    // Create epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(serverSocket);
        return 1;
    }

    // Add the server socket to epoll instance
    epoll_event event;
    event.events = EPOLLIN;  // Listen for incoming connections
    event.data.fd = serverSocket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &event) == -1) {
        perror("epoll_ctl");
        close(serverSocket);
        return 1;
    }

    // Event loop
    while (true) {
        // Wait for events
        epoll_event events[MAX_EVENTS];
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n == -1) {
            perror("epoll_wait");
            close(serverSocket);
            return 1;
        }

        for (int i = 0; i < n; ++i) {
            if (events[i].data.fd == serverSocket) {
                // New client connection
                sockaddr_in clientAddress;
                socklen_t clientAddrLen = sizeof(clientAddress);
                int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddrLen);
                if (clientSocket == -1) {
                    perror("accept");
                } else {
                    // Make client socket non-blocking
                    if (make_socket_nonblocking(clientSocket) == -1) {
                        close(clientSocket);
                    } else {
                        // Add the client socket to the epoll instance
                        event.events = EPOLLIN | EPOLLET;  // Listen for incoming data (Edge-triggered)
                        event.data.fd = clientSocket;
                        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientSocket, &event) == -1) 
                        {
                            perror("epoll_ctl");
                            close(clientSocket);
                        }
                    }
                }
            } else
            {
                // Handle client data (if any)
                char buffer[1024];
                int clientSocket = events[i].data.fd;
                int bytesRead = read(clientSocket, buffer, sizeof(buffer));
                if (bytesRead == -1) {
                    perror("read");
                } else if (bytesRead == 0) {
                    // Client closed connection
                    close(clientSocket);
                } else {
                    // Process received data and send response
                    // std::stringstream str;
                    // std::cin >> str.rdbuf();
                    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
                    send(clientSocket, response.c_str(), response.size(), 0);
                }
            }
        }
    }
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt failed");
    return 1;
    }

    close(serverSocket);
    return 0;
}

/*

notes

*   setting a FD to  non-blocking mode ensures that functions like read recv, accept will 
    return immediately is no event occurs

*   Functions like epoll, poll, or select play a crucial role in helping your 
    program determine whether a file descriptor (FD) is ready for an operation, 
    such as reading, writing, or accepting connections. 

*   Server socket event: When you add the server socket to the epoll instance, 
    you're interested in detecting new incoming client connections. 
    The server socket will only be "ready" when there is a new connection attempt. 
    When the server socket triggers an event, 
    it indicates that a client is trying to connect, so the server needs to accept that connection.

*/