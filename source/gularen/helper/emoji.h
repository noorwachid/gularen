#pragma once

#include <string>

namespace Gularen::Helper {
	const std::string& toEmoji(const std::string& shortcode);

	std::vector<std::string> getShortcodes();
}
