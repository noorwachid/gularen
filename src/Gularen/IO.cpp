#include "IO.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>

namespace Gularen
{
	namespace IO
	{
		std::string Read()
		{
			std::string buffer;
			std::array<char, 1024 * 2> bytes;

			while (std::cin)
			{
				bytes.fill(0x0);
				std::cin.read(bytes.data(), bytes.size());
				buffer += bytes.data();
			}

			return buffer;
		}

		std::string ReadLine()
		{
			std::string buffer;
			std::getline(std::cin, buffer);

			return buffer;
		}

		std::string ReadFile(const std::string& path)
		{
			std::ifstream fileStream(path);
			std::stringstream bufferStream;

			bufferStream << fileStream.rdbuf();

			return bufferStream.str();
		}

		void WriteFile(const std::string& path, const std::string& buffer)
		{
			std::ofstream fileStream(path);
			fileStream << buffer;
		}

		void Write(const std::string& buffer)
		{
			std::cout << buffer;
		}

		void WriteLine(const std::string& buffer)
		{
			std::cout << buffer << "\r\n";
		}
	}
}
