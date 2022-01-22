#include "Token.hpp"

namespace Gularen
{
	Token::Token()
		:
		Type(TokenType::unknown),
		Size(0)
	{
	}

	Token::Token(TokenType type)
		:
		Type(type),
		Size(0)
	{
	}

	Token::Token(TokenType type, size_t size)
		:
		Type(type),
		Size(size)
	{
	}

	Token::Token(TokenType type, const std::string& value)
		:
		Type(type),
		Value(value),
		Size(0)
	{
	}

	std::string Token::ToString()
	{
		std::string buffer = Gularen::toString(Type) + ":";

		if (Size > 0)
			buffer += " (" + std::to_string(Size) + ")";

		if (!Value.empty())
			buffer += " \"" + Value + "\"";

		return buffer;
	}
}
