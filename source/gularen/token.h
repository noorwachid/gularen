#pragma once

#include <string>

namespace Gularen {
	enum class TokenType {
		comment,

		indentIncr,
		indentDecr,

		bqIncr,
		bqDecr,

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

		date,
		time,
		dateTime,

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

		admonNote,
		admonHint,
		admonImportant,
		admonWarning,
		admonSeeAlso,
		admonTip,

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

		std::string toString() const;
	};

	struct Range {
		Position begin;
		Position end;
		
		std::string toString() const;
	};

	struct Token {
		TokenType type;
		Range range;
		std::string value;
		
		std::string toString() const;
	};
	
	using Tokens = std::vector<Token>;
}
