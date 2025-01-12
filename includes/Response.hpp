#pragma once
#include "../includes/Server.hpp"
#include "../includes/RequestParse.hpp"
class Server;

class Response
{
private:
    std::map<std::string, std::string> _headerMap;
    std::string                         contentType;
    std::string                         str;
    int                                 firstCall;
    int                                 size;
    std::ifstream                       file;
public:
    Response();
    ~Response();
    std::string FileToString();
    std::string getResponse(RequestParse &request, int errorFromServer);
    std::string processGetResponse(RequestParse &request);
    std::string processResponse(RequestParse &request, int isSuccess);
    std::string getHeader(RequestParse &request, const std::string &str);
    // void        getContentType(RequestParse &request);
    void        setContentType(RequestParse &request);
    void        ClearVars();
    int         getFileSize();
};

