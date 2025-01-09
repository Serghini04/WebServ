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

    if (!file.is_open())
        Utility::ShowErrorExit("Error in Open File"); // should response error 404
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
std::string Response::processResponse(RequestParse &request, int isSuccess)
{
    std::string statusLine;
    std::ostringstream bodySize;
    std::ostringstream respons;
    static int firstCall = 1;
    static int size = 0;
    static std::ifstream file;
    std::string filename;

    if (!isSuccess)
        filename = "error.html";
    else if(request.method() == ePOST)
        filename = "post.json";
    else
        filename = request._url;
    if (firstCall)
    {
        file.open(filename, std::ios::binary);
        if (file.is_open())
            size = getFileSize(file);
    }
    else
    {
        if (request.method() == eGET)
            return FileToString(file, request);
        // delete: todo
    }
    bodySize << size;
    statusLine = "HTTP/1.1 200 OK\r\n";
    _headerMap["Date"] = Utility::GetCurrentTime();
    _headerMap["Server"] = "myserver"; // get it from config file
    _headerMap["Content-Type"] = contentType;
    _headerMap["Content-Length"] = bodySize.str();
    _headerMap["Connection"] = request.getMetaData().count("Connection") == 0 ? "keep-alive" : request.getMetaData()["Connection"];
    respons << statusLine;
    for (std::map<std::string, std::string>::const_iterator
             it = _headerMap.begin();
         it != _headerMap.end(); ++it)
    {
        respons << it->first << ": " << it->second << "\r\n";
    }
    respons << "\r\n";
    firstCall = 0;
    return respons.str();
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