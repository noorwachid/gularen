#include <gularen/token.h>
#include <gularen/helper/escape.h>

namespace Gularen {
	std::string Token::toString() const {
		std::string repr = std::to_string(static_cast<int>(type)) + '\t';
		repr += std::to_string(begin.line) + ',' + std::to_string(begin.column);

		if (!(begin == end)) {
			repr += " - " + std::to_string(end.line) + ',' + std::to_string(end.column);
		}

		repr += '\t' + Helper::escape(value);

		return repr;
	}
}
