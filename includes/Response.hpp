#pragma once
#include "RequestParse.hpp"
#include <dirent.h>
#include <filesystem>
#include <sys/stat.h>
#define BUFFER_SIZE 1024 * 8


class Server;
class RequestParse;
class Conserver;


class Response
{
private:
    std::map<std::string, std::string> _headerMap;
    std::string                         contentType;
    int                                 firstCall;
    long  long                               size;
    std::ifstream                       file;
    std::string                         file_path;
    std::string                         statusLine;
    std::string                         filename;
    Conserver&                          conserver;
    static std::string                  errHtml;
    std::string                         errMsg;
    RequestParse                        *request;
    bool                                hasErrorFile;
    bool                                isDirectory;
    bool                                endResponse;
    std::string                         directoryContent;
    bool                                useChunkedEncoding;  
public:
    long                                dataSend;
    size_t                              fileSize;
    Response(Conserver &conserver, RequestParse *request);
    ~Response();
    std::string FileToString();
    std::string getResponse();
    int         processDirectory(std::string &path);
    std::string processResponse(int isSuccess);
    std::string getHeader();
    void        ProcessUrl(std::map<std::string, std::string> location);
    void        handelRequestErrors();
    void        SendError(enum status code);
    long long   getFileSize();
    int         GetErrorFromStrSize();
    bool        IsDirectory(const char *path);
    std::string handelRedirection(std::string redirect_code, std::string redirect_url);
    std::string getCgiResponse();
};


//Utils
std::string getFileHtml();
std::string replacePlaceholders(const std::string input, const std::string &placeholder, const std::string &replacement);