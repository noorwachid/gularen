#include "gularen/helper/escape.h"

namespace Gularen::Helper {
	std::string escape(const std::string& from) {
		std::string to;

		for (char c : from) {
			if (c == '\t') {
				to += "\\t";
			} else if (c == '\n') {
				to += "\\n";
			} else if (c < ' ') {
				to += "\\d" + std::to_string(c);
			} else {
				to += c;
			}
		}

		return to;
	}
}
