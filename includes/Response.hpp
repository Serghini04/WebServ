#pragma once
#include "Server.hpp"
#include "RequestParse.hpp"
#define BUFFER_SIZE 1024 * 2


class Server;
class RequestParse;
class Conserver;

enum statusCode
{
    None = 0,
    ErrNotFound = 404,
    ErrInternalError = 500
};
class Response
{
private:
    std::map<std::string, std::string> _headerMap;
    std::string                         contentType;
    std::string                         str;
    int                                 firstCall;
    int                                 size;
    std::ifstream                       file;
    std::string                         statusLine;
    std::string                         filename;
    Conserver&                          conserver;
    static std::string                  errHtml;
    std::string                        errMsg;
public:
    Response(Conserver &conserver);
    ~Response();
    std::string FileToString(RequestParse &request);
    std::string getResponse(RequestParse &request);
    std::string processResponse(RequestParse &request, int isSuccess);
    std::string getHeader(RequestParse &request, const std::string &str);
    void        handelRequestErrors(RequestParse &request);
    int         getFileSize();
};


//Utils
std::string getFileHtml();
std::string replacePlaceholders(const std::string input, const std::string &placeholder, const std::string &replacement);