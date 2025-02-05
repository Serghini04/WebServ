#include <Response.hpp>

Response::Response(Conserver &conserver, RequestParse &request) : conserver(conserver), request(request)
{
    size = 0;
    firstCall = 1;
    statusLine = "HTTP/1.1 200 OK\r\n";
    hasErrorFile = true;
    isDirectory = false;
}

Response::~Response()
{
    file.clear();
    file.close();
}
int Response::GetErrorFromStrSize()
{
    if (errMsg.empty())
    {
        errMsg = replacePlaceholders(getFileHtml(), "$MSG", request.statusCodeMessage());
        return errMsg.size();
    }
    return -1;
}

std::string Response::FileToString()
{
    if (!hasErrorFile)
    {
        hasErrorFile = !hasErrorFile;
        return errMsg;
    }
    else if (isDirectory)
    {
        isDirectory = false;
        return this->directoryContent;
    }
    if (file.is_open())
    {
        std::string buf(BUFFER_SIZE, '\0');
        file.read(&buf[0], BUFFER_SIZE);
        buf.resize(file.gcount());
        return buf;
    }
    return "";
}

int Response::getFileSize()
{
    if (!hasErrorFile)
        return GetErrorFromStrSize();
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
    if (file.fail())
        hasErrorFile = false;
    this->contentType = Utility::getExtensions("", ".html");
}

void Response::SendError(enum status code)
{
    request.SetStatusCode(code);
    if (code == eNotFound)
        request.SetStatusCodeMsg("404 Not Found1");
    else if (code == eFORBIDDEN)
        request.SetStatusCodeMsg("403 forbidden");
    handelRequestErrors();
}

void Response::ProcessUrl()
{
    std::string newUrl = conserver.getAttributes("root");
    std::map<std::string, std::string> location = conserver.getLocation(request.location());
    std::string index = "";
    std::ostringstream oss;
    
    if (!location["index"].empty() && request.URL() == "/")
        index = location["index"];
    else if (location["index"].empty() && Utility::isDirectory(request.URL()) &&
             (location["auto_index"].empty() || location["auto_index"].find("off") != std::string::npos))
        SendError(eFORBIDDEN);
    oss << newUrl << location["root"] << request.URL() << index;
    newUrl = oss.str();
    request.setUrl(newUrl);
}

std::string getFileTime(struct stat &fileInfo, const std::string &file)
{
    if (stat(file.c_str(), &fileInfo) == 0)
    {
        struct tm *time = localtime(&fileInfo.st_mtime);
        char buffer[1024];
        strftime(buffer, sizeof(buffer), "%d-%b-%Y %H:%M", time);
        return buffer;
    }
    return "Not Found";
}
int Response::processDirectory(std::string &path)
{
    std::ostringstream html;
    struct dirent *entry;
    struct stat fileInfo;

    html << "<html><head><title>Index of " << path
         << "</title></head><body>\n";
    html << "<h1>Index of " << path << "</h1>\n";
    html << "<pre>\n";
    html << "<a href=\"../\">../</a>\n";
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr)
    {
        std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
        return -1;
    }
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string filename = entry->d_name;
        std::string lasmodifies = getFileTime(fileInfo, filename);
        std::string storage = (entry->d_type == DT_DIR) ? "-" : std::to_string(fileInfo.st_size);
        if (entry->d_type == DT_DIR)
            filename += "/";

        html << "<a href=\"" << filename << "\">" << filename
             << "</a>" << std::setw(30 - filename.length()) << " "
             << lasmodifies << std::setw(20) << storage << "\n";
    }
    html << "</pre>\n</body></html>\n";
    closedir(dir);
    directoryContent = html.str();
    this->contentType = Utility::getExtensions("", ".html");
    return directoryContent.length();
}

std::string Response::processResponse(int state)
{
    if (firstCall)
    {
        if (state == 0)
            handelRequestErrors();
        else
        {
            ProcessUrl();
            if (request.statusCode() == eOK && request.method() != ePOST)
            {
                std::string str = request.URL();
                if (Utility::isDirectory(str.c_str()))
                {
                    isDirectory = true;
                    size = processDirectory(str);
                    firstCall = 0;
                    return getHeader();
                }
                file.open(str, std::ios::binary | std::ios::in);
                size_t pos = str.find(".");
                if (pos != std::string::npos)
                    this->contentType = Utility::getExtensions("", str.substr(pos));
                if (!file.is_open())
                    SendError(eNotFound);
            }
            else if (request.method() == ePOST)
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

    if (request.statusCode() != eOK 
            && request.statusCode() != eCreated)
        str = processResponse(0);
    else
        str = processResponse(1);
    return str;
}