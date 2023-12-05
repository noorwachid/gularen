#include "Gularen/Token.h"
#include "Gularen/Helper/Escape.h"

namespace Gularen
{
	std::string Position::ToString() const
	{
		return std::to_string(line) + ":" + std::to_string(column);
	}

	std::string Range::ToString() const
	{
		if (begin == end)
			return begin.ToString();

		if (begin.line == end.line)
			return std::to_string(begin.line) + ":" + std::to_string(begin.column) + "-" + std::to_string(end.column);

		return begin.ToString() + "-" + end.ToString();
	}

	std::string Token::ToString() const
	{
		std::string repr;
		size_t rangePad = 16;
		size_t typePad = 4;
		std::string rangeStr = range.ToString();
		std::string typeStr = std::to_string(static_cast<int>(type));

		repr += rangeStr + std::string(rangePad - rangeStr.size(), ' ');
		repr += typeStr + std::string(typePad - typeStr.size(), ' ');
		repr += Helper::Escape(value);

		return repr;
	}
}
