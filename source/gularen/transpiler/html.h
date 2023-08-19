#pragma once

#include <string>
#include <unordered_map>

namespace Gularen::Transpiler::HTML
{
	std::string Transpile(const std::string& content);

	// the html will injected with line sync data
	// can be accessed using attribute data-line="0"
	// its using 0-based indexing
	// index 0 refers to the first line
	std::string TranspileLS(const std::string& content);
}
