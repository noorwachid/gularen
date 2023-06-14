#pragma once

#include "DS/String.h"
#include "Integer.h"

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

        Indentation,

        Text,
        Symbol,

        Newline,

        LSQuote,
        RSQuote,
        LDQuote,
        RDQuote,

        LArrowTail,
        ArrowTail,

        XLArrow,
        LArrow,
        Arrow,
        SArrow,

        ArrowHead,

        RArrowHead,
        RArrowTail,
        RLArrowTail,

        Hyphen,
        EnDash,
        EmDash,

        Bullet,
        NumericBullet,
    };

    struct Token
    {
        TokenType type;
        String content;
        UintSize size;
        UintSize line;

        Token(TokenType type);

        Token(TokenType type, const String &content);

        Token(TokenType type, UintSize size);

        String ToString() const;

        bool operator==(const Token &other);
    };

    String ToString(TokenType tokenType);
}
