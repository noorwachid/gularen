#pragma once

#include <string>

namespace Gularen {

enum struct TokenType
{
    Unknown,

    DocumentBegin,
    DocumentEnd,

    Symbol,
    Text,
    QuotedText,

    KwImage,
    KwFile,
    KwCode,
    KwTable,
    KwAdmon,
    KwToc,

    Newline,
    Space,

    Asterisk,
    Underline,
    Backtick,

    Equal,
    Colon,
    QuestionMark,
    ExclamationMark,

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

    Teeth,   // ``
};

std::string ToString(TokenType type);

}
