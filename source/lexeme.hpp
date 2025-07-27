#pragma once

#include "Collection/String.hpp"
#include "Collection/Array.hpp"

enum TokenKind {
	TokenKind_text,
	TokenKind_newline,
	TokenKind_newlinePlus,
	TokenKind_asterisk,
	TokenKind_underscore,
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
