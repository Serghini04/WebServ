#include "ExecCGI.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <cstring>
#include <fcntl.h>
#include <sstream>
#include <stdexcept>


std::string loadHtmlFile(const std::string &filePath) {
	std::ifstream file(filePath);
	if (!file) {
	throw std::runtime_error("Failed to open HTML file: " + filePath);
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

int main() {
	const int port = 4242;
	int server_fd, client_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// Create socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
	perror("socket failed");
	return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
	perror("bind failed");
	return -1;
	}

	if (listen(server_fd, 3) < 0) {
	perror("listen failed");
	return -1;
	}

	std::cout << "Server is running on port " << port << "..." << std::endl;

	while (true) {
	if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
	perror("accept failed");
	continue;
	}

	char buffer[4096] = {0};
	read(client_fd, buffer, sizeof(buffer));
	std::string request(buffer);

	std::string response;

	std::cout << request <<std::endl;
	if (request.find("GET / ") != std::string::npos)
	{
	// Serve the HTML file
		try {
		std::string htmlContent = loadHtmlFile("index.html");
		response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + htmlContent;
		} catch (const std::exception &e) {
		response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nError: " + std::string(e.what());
		}
	}
	else if (request.find("POST") != std::string::npos) {
		// Handle POST request with CGI
		std::string body = request.substr(request.find("\r\n\r\n") + 4);
		std::string cgiPath = "/usr/bin/php"; // Adjust this path to your PHP-CGI binary
		std::string filePath = "./my-cgi/index.php"; // Path to your PHP script
		try {
		ExecCGI execCGI(cgiPath, filePath, body);
		response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n" + execCGI.GetRespose();
		} catch (const std::exception &e) {
		response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nError: " + std::string(e.what());
		}
	}
	else
	{
	// Respond with 404 for other requests
	response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found";
	}

	send(client_fd, response.c_str(), response.length(), 0);
	close(client_fd);
	}

	close(server_fd);
	return 0;
}