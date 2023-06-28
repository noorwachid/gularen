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

		reference,
		referenceID,
		referenceLabel,

		readMarker,
		includeMarker,
		jumpMarker,
		describeMarker,

		blockquote,

		admonition,

		emoji,

		minus,
		hyphen,
		enDash,
		emDash,
		
		lsquote,
		rsquote,
		ldquote,
		rdquote,
	};

	struct Token {
		TokenType type;
		size_t count;
		std::string value;
	};
	
	using Tokens = std::vector<Token>;
}