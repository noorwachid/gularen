#pragma once

#include "Integer.h"
#include "DS/String.h"

namespace Gularen
{
    enum class TokenType
    {
        BODocument,
        EODocument,

        Unknown,
        
        Asterisk,
        Underscore,
        Backtick,
        
        Text,
        Symbol,
        
        Newline,
        
        LSQuote,
        RSQuote,
        LDQuote,
        RDQuote,

        LargeArrowTail,
        ArrowTail,

        ExtraLargeArrow,
        LargeArrow,
        Arrow,
        SmallArrow,

        ArrowHead,


        Hyphen,
        EnDash,
        EmDash,
    };

    struct Token
    {
        TokenType type;
        String content;
        UintSize size;
        
        Token(TokenType type);
        
        Token(TokenType type, const String& content);

        Token(TokenType type, UintSize size);
        
        String ToString();
        
        bool operator==(const Token& other);
    };
}
