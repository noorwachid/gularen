#pragma once

#include <string>
#include <unordered_map>

namespace Gularen::Transpiler::HTML {
	std::string transpile(const std::string& content, const std::unordered_map<std::string, std::string>& options);
}
