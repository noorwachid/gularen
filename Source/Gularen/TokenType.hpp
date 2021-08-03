#pragma once

#include <string>

namespace Gularen {

enum struct TokenType
{
    Unknown,

    DocumentBegin,
    DocumentEnd,

    Text,
    Symbol,

    KwCode,
    KwTable,
    KwAd,

    Newline,
    Space,

    Asterisk,
    Underline,
    Backtick,

    Equal,

    LCurlyBracket,
    RCurlyBracket,

    Box,

    Bullet,
    NBullet,
    CheckBox,
    Line,

    Anchor,

    Tail,    // >>
    RevTail, // <<
    Arrow,   // ->

};

std::string ToString(TokenType type);

}
