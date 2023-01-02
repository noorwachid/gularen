#include "Token.h"

namespace Gularen {
    Token::Token(TokenType type): type(type), size(0) {}
    
    Token::Token(TokenType type, const String& content): type(type), content(content), size(content.size()) {}

    Token::Token(TokenType type, std::size_t size): type(type), size(size) {}

    String Token::toString() {
        switch (type) {
        case TokenType::text:
            return "text \"" + content + "\"";

        case TokenType::symbol:
            return "symbol \"" + content + "\"";

        case TokenType::newline:
            return "newline (" + std::to_string(size) + ")";
            
        case TokenType::asterisk:
            return "asterisk";
            
        case TokenType::underscore:
            return "underscore";
            
        case TokenType::backtick:
            return "backtick";
            
        case TokenType::lsQuote:
            return "lsQuote";
            
        case TokenType::rsQuote:
            return "rsQuote";
            
        case TokenType::ldQuote:
            return "ldQuote";
            
        case TokenType::rdQuote:
            return "rdQuote";

        case TokenType::boDocument:
            return "boDocument";

        case TokenType::eoDocument:
            return "eoDocument";

        case TokenType::smallArrow:
            return "smallArrow";

        case TokenType::arrow:
            return "arrow";
            
        case TokenType::largeArrow:
            return "largeArrow";
            
        case TokenType::extraLargeArrow:
            return "extraLargeArrow";

        case TokenType::arrowHead:
            return "arrowHead";

        case TokenType::arrowTail:
            return "arrowTail";
            
        case TokenType::largeArrowTail:
            return "largeArrowTail";

        default:
            return "unknown";
            break;
        }
    }  

    bool Token::operator==(const Token& other) {
        return 
            type == other.type && 
            size == other.size &&
            content == other.content
        ;
    }
}
