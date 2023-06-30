#pragma once

#include <Gularen/Token.h>
#include <vector>

namespace Gularen {
	class Lexer {
	public:
		void set(const std::string& content);

		void parse();
		
		const Tokens& get() const;

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

		void parseTable();

		void parseQuoMark(bool should, TokenType leftType, const std::string& leftValue, TokenType rightType, const std::string& rightValue);

	private:
		size_t index;
		std::string content;
		Tokens tokens;
	};
}
