#pragma once

#include <string>

namespace Gularen
{

class IO
{
public:
    IO();

    static std::string ToString(const std::string& path);

    static void Write(const std::string& buffer);
    static void Write(char c);
    static void WriteLine(const std::string& buffer);
};

}

