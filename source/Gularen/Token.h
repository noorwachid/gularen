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
