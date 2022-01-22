#include "Token.hpp"

namespace Gularen
{
	Token::Token() :
			type(TokenType::Unknown),
			size(0)
	{
	}

	Token::Token(TokenType type) :
			type(type),
			size(0)
	{

	}

	Token::Token(TokenType type, size_t size) :
			type(type),
			size(size)
	{
	}

	Token::Token(TokenType type, const std::string& value) :
			type(type),
			value(value),
			size(0)
	{
	}

	std::string Token::ToString()
	{
		std::string buffer = Gularen::ToString(type) + ":";

		if (size > 0)
			buffer += " (" + std::to_string(size) + ")";

		if (!value.empty())
			buffer += " \"" + value + "\"";

		return buffer;
	}
}
