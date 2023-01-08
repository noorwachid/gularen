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

		indentation,
        
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

        reverseArrowHead,
        reverseArrowTail,
        reverseLargeArrowTail,

        hyphen,
        enDash,
        emDash,
    };

    struct Token {
        TokenType type;
        String content;
        UintSize size;
        UintSize line;
        
        Token(TokenType type);
        
        Token(TokenType type, const String& content);

        Token(TokenType type, UintSize size);
        
        String toString() const;

        String toEscapeSequenceString() const;
        
        bool operator==(const Token& other);
    };

    String toString(TokenType tokenType);
}
