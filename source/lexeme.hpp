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
	TokenKind_heading,
	TokenKind_subheading,
	TokenKind_bullet,
	TokenKind_numberpoint,
	TokenKind_checkbox,
	TokenKind_break,
	TokenKind_hashtag,
	TokenKind_emoji,
	TokenKind_linebreak,
	TokenKind_thematicbreak,
	TokenKind_hyphen,
	TokenKind_endash,
	TokenKind_emdash,
	TokenKind_rightquote,
	TokenKind_leftquote,
	TokenKind_singlerightquote,
	TokenKind_singleleftquote,
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
