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

		codeMark,
		codeLang,
		codeSource,

		chapterMark,
		sectionMark,
		subsectionMark,

		headingID,
		headingIDMark,

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
		resourceIDMark,
		resourceID,
		resourceLabel,

		parenOpen,
		parenClose,

		presentMark,
		includeMark,
		jumpMark,
		jumpID,
		describeMark,

		admonNote,
		admonHint,
		admonImportant,
		admonWarning,
		admonSeeAlso,
		admonTip,

		emojiMark,
		emojiCode,

		hyphen,
		enDash,
		emDash,

		singleQuoteOpen,
		singleQuoteClose,
		quoteOpen,
		quoteClose,

		end,
	};

	struct Position {
		size_t line;
		size_t column;

		Position() : line(1), column(1) {
		}

		Position(size_t line, size_t column) : line(line), column(column) {
		}

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
