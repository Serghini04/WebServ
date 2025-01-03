#pragma once
#include <Server.hpp>
class Utility
{
public:
    Utility()
    {
    }
    static inline void ShowErrorExit(std::string const &errorMsg)
    {
        std::cout << errorMsg << std::endl;
        exit(1);
    }
};
