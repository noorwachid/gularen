#pragma once

#include "Collection/String.hpp"
#include "Collection/Array.hpp"

enum TokenKind {
	TokenKind_text,
	TokenKind_newline,
	TokenKind_newlines,
	TokenKind_indent,
	TokenKind_outdent,
	TokenKind_asterisk,
	TokenKind_underscore,
	TokenKind_hashtag,
	TokenKind_heading,
	TokenKind_break,
};

struct Position {
	int line;
	int column;
};

struct Range {
	Position start;
	Position end;
};

struct Token {
	TokenKind kind;
	Range range;
	String content;
};

Array<Token> lexeme(String const& source);
