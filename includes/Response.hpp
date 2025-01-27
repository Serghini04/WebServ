#pragma once
#include "Server.hpp"
#include "RequestParse.hpp"
#include <dirent.h>
#include <filesystem>
#include <sys/stat.h>
#define BUFFER_SIZE 1024 * 2


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
    std::string                         statusLine;
    std::string                         filename;
    Conserver&                          conserver;
    static std::string                  errHtml;
    std::string                         errMsg;
    RequestParse                        &request;
    bool                                hasErrorFile;
    bool                                isDirectory;
    std::string                         directoryContent;
public:
    Response(Conserver &conserver, RequestParse &request);
    ~Response();
    std::string FileToString();
    std::string getResponse();
    int         processDirectory(std::string &path);
    std::string processResponse(int isSuccess);
    std::string getHeader();
    void        ProcessUrl();
    void        handelRequestErrors();
    void        SendError(enum status code);
    int         getFileSize();
    int        GetErrorFromStrSize();
    static bool IsDirectory(const char* path);
};


//Utils
std::string getFileHtml();
std::string replacePlaceholders(const std::string input, const std::string &placeholder, const std::string &replacement);