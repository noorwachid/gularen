#include "IO.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>

namespace Gularen {
	namespace IO {
		std::string read() {
			std::string buffer;
			std::array<char, 1024 * 2> bytes;

			while (std::cin) {
				bytes.fill(0x0);
				std::cin.read(bytes.data(), bytes.size());
				buffer += bytes.data();
			}

			return buffer;
		}

		std::string readLine() {
			std::string buffer;
			std::getline(std::cin, buffer);

			return buffer;
		}

		std::string readFile(const std::string& path) {
			std::ifstream fileStream(path);
			std::stringstream bufferStream;

			bufferStream << fileStream.rdbuf();

			return bufferStream.str();
		}

		void writeFile(const std::string& path, const std::string& buffer) {
			std::ofstream fileStream(path);
			fileStream << buffer;
		}

		void write(const std::string& buffer) {
			std::cout << buffer;
		}

		void writeLine(const std::string& buffer) {
			std::cout << buffer << "\r\n";
		}
	}
}
