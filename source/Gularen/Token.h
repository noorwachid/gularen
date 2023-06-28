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

		fsBold,
		fsItalic,
		fsMonospace,

		bullet,
		checkbox,
	};

	struct Token {
		TokenType type;
		size_t count;
		std::string value;
	};
	
	void a();
}
