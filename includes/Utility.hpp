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

        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now);
        return std::string(buffer);
    }
};
