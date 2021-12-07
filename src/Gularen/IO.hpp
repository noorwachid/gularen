#pragma once

#include <string>

namespace Gularen {

    namespace IO {

        std::string read();

        std::string readLine();

        std::string readFile(const std::string& path);

        void write(const std::string& buffer);

        void writeLine(const std::string& buffer);

        void writeFile(const std::string& path, const std::string& buffer = std::string());
    };
}

