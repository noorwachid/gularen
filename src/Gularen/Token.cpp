#include "Token.h"

namespace Gularen
{
    Token::Token(TokenType type) : type(type), size(0) {}

    Token::Token(TokenType type, const String& content) : type(type), content(content), size(content.size()) {}

    Token::Token(TokenType type, std::size_t size) : type(type), size(size) {}

    String Token::ToString() const
    {
        if (type == TokenType::Text || type == TokenType::Symbol)
        {
            return Gularen::ToString(type) + " content = \"" + content + "\"";
        }

        if (type == TokenType::Indentation || type == TokenType::Newline)
        {
            return Gularen::ToString(type) + " size = " + std::to_string(size);
        }

        return Gularen::ToString(type);
    }

    bool Token::operator==(const Token& other)
    {
        return type == other.type && size == other.size && content == other.content;
    }

    String ToString(TokenType tokenType)
    {
        switch (tokenType)
        {
        case TokenType::Text:
            return "Text";
        case TokenType::Symbol:
            return "Symbol";

        case TokenType::Indentation:
            return "Indentation";
        case TokenType::Newline:
            return "Newline";

        case TokenType::Asterisk:
            return "Asterisk";
        case TokenType::Underscore:
            return "Underscore";
        case TokenType::Backtick:
            return "Backtick";

        case TokenType::LSQuote:
            return "LSQuote";
        case TokenType::RSQuote:
            return "RSQuote";
        case TokenType::LDQuote:
            return "LDQuote";
        case TokenType::RDQuote:
            return "RDQuote";

        case TokenType::BODocument:
            return "BODocument";
        case TokenType::EODocument:
            return "EODocument";

        case TokenType::SArrow:
            return "SArrow";
        case TokenType::Arrow:
            return "Arrow";
        case TokenType::LArrow:
            return "LArrow";
        case TokenType::XLArrow:
            return "XLArrow";

        case TokenType::ArrowHead:
            return "arrowHead";
        case TokenType::ArrowTail:
            return "arrowTail";
        case TokenType::LArrowTail:
            return "LArrowTail";

        case TokenType::RArrowHead:
            return "RArrowHead";
        case TokenType::RArrowTail:
            return "RArrowTail";
        case TokenType::RLArrowTail:
            return "RLArrowTail";

        case TokenType::NumericBullet:
            return "NumericBullet";
        case TokenType::Bullet:
            return "Bullet";

        default:
            return "[Unknown]";
        }
    }
}
