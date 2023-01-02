#pragma once

#include "Integer.h"
#include "DS/String.h"

namespace Gularen {
    enum class TokenType {
        boDocument,
        eoDocument,

        unknown,
        
        asterisk,
        underscore,
        backtick,
        
        text,
        symbol,
        
        newline,
        
        lsQuote,
        rsQuote,
        ldQuote,
        rdQuote,

        largeArrowTail,
        arrowTail,

        extraLargeArrow,
        largeArrow,
        arrow,
        smallArrow,

        arrowHead,


        hyphen,
        enDash,
        emDash,
    };

    struct Token {
        TokenType type;
        String content;
        UintSize size;
        
        Token(TokenType type);
        
        Token(TokenType type, const String& content);

        Token(TokenType type, UintSize size);
        
        String toString();
        
        bool operator==(const Token& other);
    };
}
