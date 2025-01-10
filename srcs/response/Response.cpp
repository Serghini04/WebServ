#include <Response.hpp>

Response::Response()
{
}
// std::string Response::processGetResponse(RequestParse &request)
// {
//     std::string data = FileToString(request);
//     this->contentType = "image/jpeg";
//     return data;
// }
// std::string Response::FileToString(std::string const &fileName)
// {
//     std::ifstream file(fileName, std::ios::binary);
//     std::ostringstream ostr;
//     std::string format;

//     if (!file.is_open())
//         Utility::ShowErrorExit("Error in Open File"); //should response error 404
//     ostr << file.rdbuf();
//     size_t pos = fileName.find(".");
//     if (pos != std::string::npos)
//         format = fileName.substr(pos + 1);
//     else
//         format = "";
//     if(format == "json")
//         this->contentType = "application/json";
//     else if(format == "html")
//         this->contentType = "text/html";
//     else
//         this->contentType = "text/plain";
//     return ostr.str();
// }

std::string Response::FileToString(std::ifstream &file, RequestParse &request)
{

    std::ostringstream ostr;
    std::string format;
    std::string buf(1028, '\0');

    file.read(&buf[0], 1028);
    size_t pos = request._url.find(".");
    if (pos != std::string::npos)
        format = request._url.substr(pos + 1);
    else
        format = "";
    if (format == "json")
        this->contentType = "application/json";
    else if (format == "html")
        this->contentType = "text/html";
    else if (format == "jpeg")
        this->contentType = "video/mp4";
    else if (format == "mp4")
        this->contentType = "image/jpeg";
    else
        this->contentType = "text/plain";
    buf.resize(file.gcount());
    return buf;
}

int getFileSize(std::ifstream &file)
{
    int size;

    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);
    return size;
}
// 1572009

std::string Response::getHeader(RequestParse &request, int size, const std::string &statusLine)
{
    std::ostringstream bodySize;
    std::ostringstream response;

    bodySize << size;
    _headerMap["Date"] = Utility::GetCurrentTime();
    _headerMap["Server"] = "myserver"; // get it from config file
    _headerMap["Content-Type"] = contentType;
    _headerMap["Content-Length"] = bodySize.str();
    _headerMap["Connection"] = request.getMetaData().count("Connection") == 0 ? "keep-alive" : request.getMetaData()["Connection"];
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
    static int firstCall = 1;
    static int size = 0;
    static std::ifstream file;
    std::string filename;

    statusLine = "HTTP/1.1 200 OK\r\n";
    if (!isSuccess)
    {
        filename = "error.html";
        statusLine = "HTTP/1.1 400 Bad Request\r\n";
    }
    else if(firstCall && request.method() == ePOST)
        file.open("post.json");
    else
        filename = request._url;
    if (firstCall)
    {
        if(request.method() != ePOST)
            file.open(filename, std::ios::binary);
        if (!file.is_open())
        {
            statusLine = "HTTP/1.1 400 Bad Request\r\n";
            file.open("error.html");
        }
        size = getFileSize(file);
    }
    else
        return FileToString(file, request);
    firstCall = 0;
    return getHeader(request, size, statusLine);
}

std::string Response::getResponse(RequestParse &request)
{
    std::string str;

    if (request.statusCode() != eOK)
        str = processResponse(request, 0);
    else
        str = processResponse(request, 1);
    return str;
}