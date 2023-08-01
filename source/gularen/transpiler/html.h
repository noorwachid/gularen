#pragma once

#include <string>
#include <unordered_map>

namespace Gularen::Transpiler::HTML {
	std::string transpile(const std::string& content);

	std::string transpileWithRange(const std::string& content);
}
