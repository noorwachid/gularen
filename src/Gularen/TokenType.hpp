#pragma once

#include <string>

namespace Gularen {
	enum struct TokenType {
		unknown,

		openDocument,
		closeDocument,

		hash,
		at,
		symbol,

		string,

		openFormatting,
		closeFormatting,

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

		period,
		comma,
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
		openSquareBracket,
		closeSquareBracket,

		dollar,

		bullet,
		numericBullet,
		checkBox,
		line,

		anchor,

		tail, // >>
		reverseTail, // <<
		arrow, // ->
	};

	std::string toString(TokenType type);
}
