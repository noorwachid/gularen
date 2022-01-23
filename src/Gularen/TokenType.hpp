#pragma once

#include <string>

namespace Gularen
{
	enum struct TokenType
	{
		unknown,

		openDocument,
		closeDocument,

		hash,
		at,
		symbol,

		buffer,

		openInterpolation,
		closeInterpolation,

		tableKeyword,
		codeKeyword,
		admonitionKeyword,
		fileKeyword,
		imageKeyword,
		tocKeyword,
		indexKeyword,
		referenceKeyword,

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
		caret,
		ampersand,

		// English based stylistic quotes
		openSingleQuote,
		closeSingleQuote,
		openDoubleQuote,
		closeDoubleQuote,

		openCurlyBracket,
		closeCurlyBracket,

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
