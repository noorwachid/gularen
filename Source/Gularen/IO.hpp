#pragma once

#include <string>

namespace Gularen
{

class IO
{
public:
    IO();

    static std::string ReadFile(const std::string& path);
    static void WriteFile(const std::string& path, const std::string& buffer = std::string());

    static void Write(const std::string& buffer);
    static void Write(char c);
    static void WriteLine(const std::string& buffer);
};

}

