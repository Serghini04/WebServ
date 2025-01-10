#pragma once
#include "../includes/Server.hpp"
#include "../includes/RequestParse.hpp"
class Server;
class Response
{
private:
    std::map<std::string, std::string> _headerMap;
    std::string contentType;
public:
    Response();
    std::string FileToString(std::ifstream &file,RequestParse &request);
    std::string getResponse(RequestParse &request);
    std::string processGetResponse(RequestParse &request);
    std::string processResponse(RequestParse &request, int isSuccess);
    std::string getHeader(RequestParse &request, int size, const std::string &str);
};

