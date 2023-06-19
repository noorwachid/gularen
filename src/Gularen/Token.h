#pragma once

#include "DS/String.h"
#include "Integer.h"

namespace Gularen {
	enum class TokenType {
		boDocument,
		eoDocument,

		unknown,

		asterisk,
		underscore,
		Backtick,

		indentation,

		text,
		symbol,

		newline,

		lsQuote,
		rsQuote,
		ldQuote,
		rdQuote,

		lArrowTail,
		arrowTail,

		xlArrow,
		lArrow,
		arrow,
		sArrow,

		arrowHead,

		rArrowHead,
		rArrowTail,
		rlArrowTail,

		hyphen,
		enDash,
		emDash,

		bullet,
		numericBullet,
	};

	struct Token {
		TokenType type;
		String content;
		UintSize size;
		UintSize line;

		Token(TokenType type);

		Token(TokenType type, const String& content);

		Token(TokenType type, UintSize size);

		String toString() const;

		bool operator==(const Token& other);
	};

	String toString(TokenType tokenType);
}
