#pragma once

#include <string>

namespace Gularen
{
	enum struct TokenType
	{
		Unknown,

		DocumentBegin,
		DocumentEnd,

		HashSymbol,
		AtSymbol,
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
		Pipe,
		QuestionMark,
		ExclamationMark,

		LCurlyBracket,
		RCurlyBracket,

		Dollar,

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
