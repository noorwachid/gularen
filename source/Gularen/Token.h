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

		resource,
		resourceID,
		resourceLabel,

		presentMarker,
		includeMarker,
		jumpMarker,
		jumpID,
		describeMarker,

		blockquote,

		admon,

		emojiDeli,
		emoji,

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
	};

	struct Token {
		TokenType type;
		Position begin;
		Position end;
		std::string value;
		
		std::string toString() const {
			return std::to_string(static_cast<int>(type)) + 
				'\t' + std::to_string(begin.line) + ',' + std::to_string(begin.column) + 
				" - " + std::to_string(end.line) + ',' + std::to_string(end.column) + 
				'\t' + value;
		}
	};
	
	using Tokens = std::vector<Token>;
}
