#include <Response.hpp>

Response::Response()
{
    size = 0;
    file.clear();
    firstCall = 1;
    this->contentType = "";
}

Response::~Response()
{
    
}

std::string Response::FileToString()
{
    std::string buf(1024, '\0');

    file.read(&buf[0], 1024);
    buf.resize(file.gcount());
    return buf;
}
void Response::setContentType(RequestParse &request)
{
    std::string format;

    Utility::Debug(request._url);
    if( request._url.empty())
        return ;
    size_t pos = request._url.find(".");
    if (pos != std::string::npos)
        format = request._url.substr(pos + 1);
    else
        format = "";
    if (format == "json")
        this->contentType = "application/json";
    else if (format == "html")
        this->contentType = "text/html";
    else if (format == "mp4")
        this->contentType = "video/mp4";
    else if (format == "jpeg")
        this->contentType = "image/jpeg; charset=UTF-8";
    else
        this->contentType = "text/plain";
}
int Response::getFileSize()
{
    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);
    return size;
}


std::string Response::getHeader(RequestParse &request, const std::string &statusLine)
{
    std::ostringstream bodySize;
    std::ostringstream response;

    bodySize << size;
    _headerMap["Date"] = Utility::GetCurrentTime();
    _headerMap["Server"] = "myserver"; // get it from config file
    _headerMap["Content-Type"] = contentType;
    _headerMap["Content-Length"] = bodySize.str();
    _headerMap["Connection"] = request.getMetaData().count("Connection") == 0 ? "keep-alive" : request.getMetaData()["Connection"];
    // _headerMap["Connection"] = "close";
    response << statusLine;
    for (std::map<std::string, std::string>::const_iterator
             it = _headerMap.begin();
         it != _headerMap.end(); ++it)
    {
        response << it->first << ": " << it->second << "\r\n";
    }
    response << "\r\n";
    return response.str();
}

std::string Response::processResponse(RequestParse &request, int isSuccess)
{
    std::string statusLine;
    std::string filename;

    statusLine = "HTTP/1.1 200 OK\r\n";
    if (isSuccess <= 0)
    {
        this->contentType = "text/html";
        filename = "error.html";
        statusLine = "HTTP/1.1 400 Bad Request\r\n";
    }
    else if (firstCall && request.method() == ePOST)
    {
        file.open("post.json");
        this->contentType = "application/json";
    }
    else
    {
        setContentType(request);
        filename = request._url;
    }
    if (firstCall)
    {
        if (request.method() != ePOST)
            file.open(filename, std::ios::binary);
        if (!file.is_open())
        {
            statusLine = "HTTP/1.1 400 Bad Request\r\n";
            file.open("error.html");
        }
        size = getFileSize();
    }
    else
        return FileToString();
    firstCall = 0;
    return getHeader(request, statusLine);
}

std::string Response::getResponse(RequestParse &request, int state)
{
    std::string str;
    if(state)
    {
        str = processResponse(request, -1);
        std::cout << str << std::endl;
    }
    if (request.statusCode() != eOK)
        str = processResponse(request, 0);
    else
        str = processResponse(request, 1);
    return str;
}
