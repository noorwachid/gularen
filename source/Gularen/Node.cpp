#include <Gularen/Node.h>

void escapeYAML(const std::string& from, std::string& to) {
	for (char c : from) {
		if (c == '\t') {
			to += "\\t";
			} else if (c == '\n') {
			to += "\\n";
			} else if (c == '{') {
			to += "\\{";
			} else if (c == '}') {
			to += "\\}";
			} else if (c == '[') {
			to += "\\[";
			} else if (c == ']') {
			to += "\\]";
			} else if (c < ' ') {
			to += "\\d" + std::to_string(c);
			} else {
			to += c;
		}
	}
}

