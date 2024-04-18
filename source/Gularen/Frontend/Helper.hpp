#pragma once

#include <string_view>

namespace Gularen {

class Helper {
public:
	static std::string_view trim(std::string_view input) {
		size_t first = input.find_first_not_of(' ');
		if (std::string_view::npos == first) {
			return input;
		}

		size_t last = input.find_last_not_of(' ');
		return input.substr(first, (last - first + 1));
	}
};

}
