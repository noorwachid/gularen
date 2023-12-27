#include "Gularen/parse.h"
#include "Gularen/Internal/Parser.h"

namespace Gularen {
	NodePtr parse(const std::string& content) {
		Parser parser;
		parser.set(content);
		parser.parse();
		return parser.getRoot();
	}

	NodePtr parseFile(const std::string& path) {
		Parser parser;
		parser.load(path);
		parser.parse();
		return parser.getRoot();
	}
}
