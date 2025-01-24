#include <Response.hpp>

Response::Response(Conserver &conserver, RequestParse &request) : conserver(conserver), request(request)
{
    size = 0;
    file.clear();
    firstCall = 1;
    statusLine = "HTTP/1.1 200 OK\r\n";
}

Response::~Response()
{
}

std::string Response::FileToString()
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

std::string Response::getHeader()
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
    _headerMap["Keep-Alive"] = "timeout=5, max = 100";
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

void Response::handelRequestErrors()
{
    statusLine = "HTTP/1.1 " + request.statusCodeMessage() + "\r\n";
    file.open(conserver.getErrorPage(request.statusCode()));
    this->contentType = Utility::getExtensions("", ".html");
}

void Response::NotFoundError()
{
    request.SetStatusCode(eNotFound);
    request.SetStatusCodeMsg("404 Not Found");
    handelRequestErrors();
}
void Response::CheckConfig()
{
    std::map<std::string, std::string> mapLocation = conserver.getLocation(request.URL());
    if (!mapLocation.empty() && mapLocation["allowed_methods"].find("GET") != std::string::npos)
    {
        request.setUrl(mapLocation["root"]);
    }
    else
        NotFoundError();
}
std::string Response::processResponse(int state)
{
    if (firstCall)
    {
        if (state == 0)
            handelRequestErrors();
        else
        {
            CheckConfig();
            if (request.statusCode()== eOK && request.method() != ePOST)
            {
                file.open(request.URL(), std::ios::binary | std::ios::in);
                size_t pos = request.URL().find(".");
                if (pos != std::string::npos)
                {
                    std::string str = request.URL();
                    str.erase(str.end() - 1);
                    this->contentType = Utility::getExtensions("", str.substr(pos));
                }
                if (!file.is_open())
                    NotFoundError();
            }
            else if (request.statusCode() == eOK)
            {
                file.open("post.json");
                this->contentType = Utility::getExtensions("", ".json");
            }
        }
        size = getFileSize();
    }
    else
        return FileToString();
    firstCall = 0;
    return getHeader();
}

std::string Response::getResponse()
{
    std::string str;

    if (request.statusCode() != eOK)
        str = processResponse(0);
    else
        str = processResponse(1);
    return str;
}
