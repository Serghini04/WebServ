#include <Response.hpp>

Response::Response()
{

}

std::string Response::FileToString(std::string const &fileName)
{
    std::ifstream file(fileName, std::ios::binary);
    std::ostringstream ostr;
    std::string format;

    if (!file.is_open())
        Utility::ShowErrorExit("Error in Open File"); //should response error 404
    ostr << file.rdbuf();
    size_t pos = fileName.find(".");
    ostr.flush();
    if (pos != std::string::npos)
        format = fileName.substr(pos + 1); 
    else
        format = ""; 
    if(format == "json")
        this->contentType = "application/json";
    else if(format == "html")
        this->contentType = "text/html";
    else
        this->contentType = "text/plain";
    return ostr.str();
}
std::string Response::processGetResponse(RequestParse &request)
{
    std::string data = FileToString(request._url);
    this->contentType = "image/jpeg";
    return data;
}

std::string Response::processResponse(RequestParse &request, int isSuccess)
{
    std::string statusLine;
    std::string body;
    std::ostringstream bodySize;
    std::ostringstream respons;

    if (!isSuccess)
        body = FileToString("error.html");
    else
    {
        if (request.method() == ePOST)
            body = FileToString("post.json");
        else if (request.method() == eGET)
            body = processGetResponse(request);
        // delete: todo
    }
    bodySize << body.size();
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
    respons << body;
    std::ofstream mfile("test");
    mfile << respons.str();
    mfile.flush();
    return respons.str();
}


// std::string Response::getHeader(RequestParse &request)
// {
//     std::ostringstream oss;
//     std::string responseBody;
//     std::string header;
//     oss << responseBody.size();

//     header = "HTTP/1.1 200 OK";
//     _headerMap["Date"] = Utility::GetCurrentTime();
//     _headerMap["Server"] = _requestData.getMetaData()["Server"];
//     _headerMap["Content-Type"] = _requestData.getMetaData()["Content-Type"];
//     header =
//         "HTTP/1.1 200 OK\r\n"
//         "Content-Type: text/html\r\n"
//         "Content-Length: " +
//         oss.str() + "\r\n"
//                     "\r\n";
//     return header;
// }

std::string Response::getResponse(RequestParse &request)
{
    if (request.statusCode() != eOK)
        return processResponse(request, 0);
    else
        return processResponse(request, 1);
}