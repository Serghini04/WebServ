#include <Response.hpp>

Response::Response(Conserver &conserver) : conserver(conserver)
{
    size = 0;
    file.clear();
    firstCall = 1;
    statusLine = "HTTP/1.1 200 OK\r\n";
}

Response::~Response()
{
    
}

std::string Response::FileToString(RequestParse &request)
{
    if (request.statusCode() != eOK && !file.is_open())
    {
        if (errMsg.empty())
        {
            errMsg = replacePlaceholders(getFileHtml(), "$MSG", request.statusCodeMessage());
            return errMsg;
        }
        return "";
    }
    std::string buf(BUFFER_SIZE, '\0');
    file.read(&buf[0], BUFFER_SIZE);
    buf.resize(file.gcount());
    return buf;
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
    std::string ser = conserver.getAttributes("server_name");
    ser.erase(ser.end() - 1);
    _headerMap["Server"] = ser;
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

void Response::handelRequestErrors(RequestParse &request)
{
    statusLine = "HTTP/1.1 " + request.statusCodeMessage() + "\r\n";
    file.open(conserver.getErrorPage(request.statusCode()));
    this->contentType = Utility::getExtensions("", ".html");
}

std::string Response::processResponse(RequestParse &request, int state)
{
    if (firstCall)
    {
        if (state == 0)
            handelRequestErrors(request);
        else
        {
            if (request.method() != ePOST)
            {
                file.open(request.URL(), std::ios::binary | std::ios::in);
                size_t pos = request.URL().find(".");
                if (pos != std::string::npos)
                    this->contentType = Utility::getExtensions("", request.URL().substr(pos));
                if (!file.is_open())
                {
                    request.SetStatusCode(eNotFound);
                    request.SetStatusCodeMsg("404 Not Found");
                    handelRequestErrors(request);
                }
            }else
            {
                file.open("post.json");
                this->contentType = Utility::getExtensions("", ".json");
            }
        }
        size = getFileSize();
    }
    else
        return FileToString(request);
    firstCall = 0;
    return getHeader(request, statusLine);
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
