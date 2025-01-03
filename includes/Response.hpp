#pragma once
#include "../includes/Server.hpp"

class Server;
class Response
{
private:

public:
    Response();
    static std::string FileToString(std::string const &fileName);
};

