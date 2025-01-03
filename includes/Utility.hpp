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
};
