#pragma once

#include <string>

namespace Gularen {
	enum class TokenType {
		indent,
		newline,

		text,

		codeMarker,
		codeLang,
		codeSource,
		
		headingMarker,
		headingID,
		headingIDMarker,

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
		describeMarker,

		blockquote,

		admon,

		emoji,

		minus,
		hyphen,
		enDash,
		
		lsQuo,
		rsQuo,
		ldQuo,
		rdQuo,
	};

	struct Token {
		TokenType type;
		size_t count;
		std::string value;
		
		std::string toString() const {
			return std::to_string(static_cast<int>(type)) + '\t' + std::to_string(count) + '\t' + value;
		}
	};
	
	using Tokens = std::vector<Token>;
}
