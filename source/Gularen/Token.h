#pragma once

#include <string>

namespace Gularen {
	enum class TokenType {
		comment,

		indentIncr,
		indentDecr,

		newline,
		newlinePlus,

		text,

		codeMarker,
		codeLang,
		codeSource,
		
		chapterOper,
		sectionOper,
		subsectionOper,
	
		headingID,
		headingIDOper,

		fsBold,
		fsItalic,
		fsMonospace,

		bullet,
		checkbox,
		index,

		break_,
		dinkus,

		pipe,
		pipeConnector,

		squareOpen,
		squareClose,

		curlyOpen,
		curlyClose,

		resource,
		resourceIDMarker,
		resourceID,

		parenOpen,
		parenClose,

		resourceLabel,

		presentMarker,
		includeMarker,
		jumpMarker,
		jumpID,
		describeMarker,

		blockquote,

		admonNote,
		admonHint,
		admonImportant,
		admonWarning,
		admonDanger,
		admonSeeAlso,

		emojiMarker,
		emojiCode,

		hyphen,
		enDash,
		emDash,
		
		lsQuo,
		rsQuo,
		ldQuo,
		rdQuo,

		eof,
	};

	struct Position {
		size_t line;
		size_t column;

		Position() = default;

		Position(size_t line, size_t column) : line{line}, column{column} {}

		bool operator==(const Position& other) const {
			return line == other.line && column == other.column;
		}
	};

	struct Token {
		TokenType type;
		Position begin;
		Position end;
		std::string value;
		
		std::string toString() const;
	};
	
	using Tokens = std::vector<Token>;
}
