#pragma once
#include "../includes/Server.hpp"
#include "../includes/RequestParse.hpp"
class Server;
class RequestParse;
class Conserver;
class Response
{
private:
    std::map<std::string, std::string> _headerMap;
    std::string                         contentType;
    std::string                         str;
    int                                 firstCall;
    int                                 size;
    std::ifstream                       file;
    Conserver&                          conserver;
public:
    Response(Conserver &conserver);
    ~Response();
    std::string FileToString();
    std::string getResponse(RequestParse &request, int errorFromServer);
    std::string processResponse(RequestParse &request, int isSuccess);
    std::string getHeader(RequestParse &request, const std::string &str);
    void        setContentType(RequestParse &request);
    int         getFileSize();
};

