#include "Token.h"

namespace Gularen 
{
    Token::Token(TokenType type): type(type), size(0) {}
    
    Token::Token(TokenType type, const String& content): type(type), content(content), size(content.size()) {}

    Token::Token(TokenType type, std::size_t size): type(type), size(size) {}

    String Token::ToString()
    {
        switch (type)
        {
        case TokenType::Text:
            return "Text (" + std::to_string(size) + ") \"" + content + "\"";

        case TokenType::Newline:
            return "Newline (" + std::to_string(size) + ")";
            
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
            
        default:
            return "Unknown";
            break;
        }
    }  

    bool Token::operator==(const Token& other)
    {
        return 
            type == other.type && 
            size == other.size &&
            content == other.content
        ;
    }
}
