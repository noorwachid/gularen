#include "Gularen/Internal/Token.h"
#include "Gularen/Internal/Helper/escape.h"

namespace Gularen {
	std::string Position::toString() const {
		return std::to_string(line) + ":" + std::to_string(column);
	}

	std::string Range::toString() const {
		if (begin == end)
			return begin.toString();

		if (begin.line == end.line)
			return std::to_string(begin.line) + ":" + std::to_string(begin.column) + "-" + std::to_string(end.column);

		return begin.toString() + "-" + end.toString();
	}

	std::string Token::toString() const {
		std::string repr;
		size_t rangePad = 16;
		size_t typePad = 4;
		std::string rangeStr = range.toString();
		std::string typeStr = std::to_string(static_cast<int>(type));

		repr += rangeStr + std::string(rangePad - rangeStr.size(), ' ');
		repr += typeStr + std::string(typePad - typeStr.size(), ' ');
		repr += Helper::escape(value);

		return repr;
	}
}
