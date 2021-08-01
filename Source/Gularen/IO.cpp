#include "IO.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace Gularen
{

IO::IO()
{

}

std::string IO::ToString(const std::string& path)
{
    std::ifstream fileStream(path);
    std::stringstream bufferStream;

    bufferStream << fileStream.rdbuf();

    return bufferStream.str();
}

void IO::Write(char c)
{
    std::cout << c;
}

void IO::Write(const std::string& buffer)
{
    std::cout << buffer;
}

void IO::WriteLine(const std::string& buffer)
{
    std::cout << buffer << "\r\n";
}

}
