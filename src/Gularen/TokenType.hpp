#pragma once

#include <string>

namespace Gularen
{
	enum struct TokenType
	{
		unknown,

		documentBegin,
		documentEnd,

		hashSymbol,
		atSymbol,
		symbol,
		text,
		quotedText,
		rawText,

		keywordImage,
		keywordTable,
		keywordCode,
		KeywordBlock,
		KeywordFile,
		KeywordToc,
		KeywordRef,

		newline,
		space,

		asterisk,
		underline,
		backtick,

		equal,
		colon,
		pipe,
		questionMark,
		exclamationMark,

		leftCurlyBracket,
		rightCurlyBracket,

		dollar,

		bullet,
		numericBullet,
		checkBox,
		line,

		anchor,

		tail,    // >>
		reverseTail, // <<
		arrow,   // ->

		teeth,   // ``
	};

	std::string toString(TokenType type);
}
