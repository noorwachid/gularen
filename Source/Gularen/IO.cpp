#include "IO.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace Gularen {

IO::IO()
{
}

std::string IO::Read()
{
    std::string buffer;
    char partialBuffer[1024 * 2];

    while (std::cin)
    {
        std::cin.read(partialBuffer, 1024 * 2);
        buffer += partialBuffer;
    }

    return buffer;
}

std::string IO::ReadLine()
{
    std::string buffer;
    std::getline(std::cin, buffer);

    return buffer;
}

std::string IO::ReadFile(const std::string& path)
{
    std::ifstream fileStream(path);
    std::stringstream bufferStream;

    bufferStream << fileStream.rdbuf();

    return bufferStream.str();
}

void IO::WriteFile(const std::string& path, const std::string& buffer)
{
    std::ofstream fileStream(path);
    fileStream << buffer;
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
