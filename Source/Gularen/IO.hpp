#pragma once

#include <string>

namespace Gularen
{

class IO
{
public:
    IO();

    static std::string Read();
    static std::string ReadLine();
    static std::string ReadFile(const std::string& path);

    static void Write(const std::string& buffer);
    static void WriteLine(const std::string& buffer);
    static void WriteFile(const std::string& path, const std::string& buffer = std::string());
};

}

