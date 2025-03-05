#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/event.h>
#include <cstring>

#define PORT 8080
#define MAX_EVENTS 10

int main() {
    int server_fd, client_fd, kq;
    struct sockaddr_in server_addr;
    struct kevent change_event, event_list[MAX_EVENTS];

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket failed");
        return 1;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        close(server_fd);
        return 1;
    }

    // Bind the socket to an address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    // Start listening
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        close(server_fd);
        return 1;
    }

    // Create a kqueue instance
    kq = kqueue();
    if (kq == -1) {
        perror("kqueue failed");
        close(server_fd);
        return 1;
    }

    // Register the server socket with kqueue for read events
    EV_SET(&change_event, server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
        perror("kevent failed");
        close(server_fd);
        close(kq);
        return 1;
    }

    // Start event loop
    while (true) {
        int num_events = kevent(kq, NULL, 0, event_list, MAX_EVENTS, NULL);
        if (num_events == -1) {
            perror("kevent failed");
            break;
        }

        // Process each event
        for (int i = 0; i < num_events; i++) {
            if (event_list[i].ident == server_fd) {
                // New connection
                client_fd = accept(server_fd, NULL, NULL);
                if (client_fd == -1) {
                    perror("Accept failed");
                    continue;
                }

                // Make the client socket non-blocking
                fcntl(client_fd, F_SETFL, O_NONBLOCK);

                // Register the client socket with kqueue for read events
                EV_SET(&change_event, client_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                if (kevent(kq, &change_event, 1, NULL, 0, NULL) == -1) {
                    perror("kevent failed");
                    close(client_fd);
                    continue;
                }

                std::cout << "New connection accepted!" << std::endl;

            } else if (event_list[i].filter == EVFILT_READ) {
                // Data available to read from client
                char buffer[1024];
                ssize_t bytes_read = read(event_list[i].ident, buffer, sizeof(buffer));

                if (bytes_read <= 0) {
                    // Client disconnected or error occurred
                    if (bytes_read == 0) {
                        std::cout << "Client disconnected." << std::endl;
                    } else {
                        perror("Read error");
                    }

                    // Close the client socket and deregister it from kqueue
                    close(event_list[i].ident);
                    EV_SET(&change_event, event_list[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(kq, &change_event, 1, NULL, 0, NULL);
                } else {
                    // Send response to client (simple HTTP response)
                    const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
                    send(event_list[i].ident, response, strlen(response), 0);
                    close(event_list[i].ident);
                    EV_SET(&change_event, event_list[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                    kevent(kq, &change_event, 1, NULL, 0, NULL);
                }
            }
        }
    }

    // Clean up
    close(server_fd);
    close(kq);

    return 0;
}
