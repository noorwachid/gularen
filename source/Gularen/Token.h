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

		break_,
		dinkus,
	};

	struct Token {
		TokenType type;
		size_t count;
		std::string value;
	};
	
	void a();
}
