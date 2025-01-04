#pragma once
#include "../includes/Server.hpp"
#include "../includes/RequestParse.hpp"
class Server;
class Response
{
private:
    std::string getHeader(RequestParse &request);
public:
    Response();
    static std::string FileToString(std::string const &fileName);

};

