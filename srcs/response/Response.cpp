#include <Response.hpp>
Response::Response()
{

}

std::string Response::FileToString(std::string const &fileName)
{
    std::ifstream file(fileName);
    std::ostringstream ostr;

    if(!file.is_open())
        Utility::ShowErrorExit("Error in Open File");
    ostr << file.rdbuf();
    return ostr.str(); 
}

std::string Response::getHeader(RequestParse &request)
{
    std::ostringstream oss;
    std::string responseBody;
    std::string header;
    oss << responseBody.size();
    std::map<std::string, std::string> headerMap;
    if(request._state == eOK)
        header = "HTTP/1.1 200 OK";
    else
        header = "HTTP/1.1 400 Bad";
    headerMap["Date"] = Utility::GetCurrentTime();
    headerMap["Server"] = request.getMetaData()["Server"];
    headerMap["Content-Type"] = request.getMetaData()["Content-Type"];
    header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " +
        oss.str() + "\r\n"
                    "\r\n";
    return header;
}