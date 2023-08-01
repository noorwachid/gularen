#include <gularen/token.h>
#include <gularen/helper/escape.h>

namespace Gularen {
	std::string Position::toString() const {
		return std::to_string(line) + ":" + std::to_string(column);
	}

	std::string Range::toString() const {
		if (begin == end) {
			return begin.toString();
		}

		return begin.toString() + "-" + end.toString();
	}

	std::string Token::toString() const {
		std::string repr;
		size_t rangePad = 16;
		size_t typePad = 4;
		std::string rangeS = range.toString();
		std::string typeS = std::to_string(static_cast<int>(type));

		repr += rangeS + std::string(rangePad - rangeS.size(), ' ');
		repr += typeS + std::string(typePad - typeS.size(), ' ');
		repr += Helper::escape(value);

		return repr;
	}
}
