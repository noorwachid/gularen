#include "Gularen/Internal/Helper/quote.h"

namespace Gularen::Helper {
	std::string quote(const std::string& from) {
		std::string to = "\"";

		for (char c : from) {
			if (c == '\t')
				to += "\\t";
			else if (c == '\n')
				to += "\\n";
			else if (c < ' ')
				to += "\\d" + std::to_string(c);
			else if (c == '"')
				to += "\\\"";
			else
				to += c;
		}

		to += "\"";

		return to;
	}
}
