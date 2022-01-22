#pragma once

#include <string>

namespace Gularen
{
	namespace IO
	{
		std::string Read();

		std::string ReadLine();

		std::string ReadFile(const std::string& path);

		void Write(const std::string& buffer);

		void WriteLine(const std::string& buffer);

		void WriteFile(const std::string& path, const std::string& buffer = std::string());
	};
}

