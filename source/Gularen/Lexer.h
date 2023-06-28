#pragma once

#include <Gularen/Token.h>
#include <vector>

namespace Gularen {
	using Tokens = std::vector<Token>;

	class Lexer {
	public:
		const Tokens& parse(const std::string& content);

	private:
		bool check(size_t offset);

		bool is(size_t offset, char c);

		char get(size_t offset);

		void advance(size_t offset);

		size_t count(char c);

		void add(TokenType type, size_t count, const std::string& value);

		void addText(const std::string value);

		void parseBlock();
		
		void parseCode();

		void parseInline();

		void parseText();

		void parseSpace();

	private:
		size_t index;
		std::string content;
		Tokens tokens;
	};
}
