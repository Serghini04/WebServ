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