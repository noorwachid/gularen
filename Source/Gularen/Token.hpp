#pragma once

#include "TokenType.hpp"

namespace Gularen {

struct Token
{
    Token();
    Token(TokenType type);
    Token(TokenType type, size_t size);
    Token(TokenType type, const std::string& value);

    std::string ToString();

    TokenType type;
    std::string value;
    size_t size;
};

}

