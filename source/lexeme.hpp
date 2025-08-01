#pragma once

#include "Collection/String.hpp"
#include "Collection/Array.hpp"

enum TokenKind {
	TokenKind_text, // abc
	TokenKind_escape, // \*
	TokenKind_newline, // \n
	TokenKind_newlines, // \n\n
	TokenKind_indent, // |->
	TokenKind_outdent, // <-|
	TokenKind_asterisk, // *
	TokenKind_underscore, // _
	TokenKind_heading, // >>> or >> or >
	TokenKind_colon, // :
	TokenKind_bullet, // - 
	TokenKind_numberpoint, // 1. 
	TokenKind_checkbox, // [ ] or [x] 
	TokenKind_hashtag, // #xxx
	TokenKind_emoji, // :xxx:
	TokenKind_linebreak, // <
	TokenKind_thematicbreak, // ---
	TokenKind_hyphen, // -
	TokenKind_endash, // --
	TokenKind_emdash, // ---
	TokenKind_rightquote, // "
	TokenKind_leftquote, // "
	TokenKind_singlerightquote, // '
	TokenKind_singleleftquote, // '

	TokenKind_openref, // [ or ![ or ^[
	TokenKind_ref, // xxx
	TokenKind_quotedref, // "xxx"
	TokenKind_closeref, // ]
	TokenKind_footnote, // ^
	TokenKind_openlabel, // (
	TokenKind_closelabel, // )

	TokenKind_openfence, // ```
	TokenKind_closefence, // ```
	TokenKind_sources, // goto 1 (multiline)
	TokenKind_backtick, // `
	TokenKind_source, // goto 1
	TokenKind_id, // cpp [a-z0-9-]+

	TokenKind_pipe, // |
	TokenKind_bar, // --- or +-- or +-+ or --+

	TokenKind_admon, // [NOTE] [HINT] [IMPORTANT] [WARNING] [SEE] [TIP]
	TokenKind_citation, // ^ xxx

	TokenKind_script, // % 
	TokenKind_func, // include
	TokenKind_argument, // xxx
	TokenKind_quotedargument, // "xxx"
	TokenKind_value, // value
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
