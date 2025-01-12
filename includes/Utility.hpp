#pragma once
#include <Server.hpp>
class Utility
{
public:
    Utility()
    {
    }
    static void ShowErrorExit(std::string const &errorMsg)
    {
        std::cout << errorMsg << std::endl;
        exit(1);
    }
    static std::string GetCurrentTime()
    {
        std::time_t t = std::time(0); 
        std::tm* now = std::localtime(&t);
        char buffer[80];

        std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", now);
        return std::string(buffer);
    }
    static void Debug(std::string str)
    {
        std::cout << "----------Debug------------" << "\n";
        std::cout << str << "\n";
        std::cout << "----------Debug------------" << "\n";
        // exit(0);
    }
};
