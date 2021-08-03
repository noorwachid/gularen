#include "Token.hpp"

namespace Gularen {

Token::Token():
    type(TokenType::Unknown),
    size(0)
{
}

Token::Token(TokenType type):
    type(type),
    size(0)
{
}

Token::Token(TokenType type, const std::string& value):
    type(type),
    value(value),
    size(0)
{
}

std::string Token::ToString()
{
    std::string buffer = Gularen::ToString(type) + ": ";

    if (type == TokenType::Text)
        buffer += "\"" + value + "\"";

    if (type == TokenType::Newline ||
        type == TokenType::Space ||
        type == TokenType::Bullet ||
        type == TokenType::CheckBox)

        buffer += "(" + std::to_string(size) + ")";

    return buffer;
}

}
