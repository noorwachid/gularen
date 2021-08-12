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
    RawText,

    KwImage,
    KwTable,
    KwCode,
    KwAdmon,
    KwFile,
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
