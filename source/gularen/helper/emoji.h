#pragma once

#include <string>

namespace Gularen::Helper
{
	const std::string& ShortcodeToEmoji(const std::string& shortcode);

	std::vector<std::string> GetShortcodes();
}
