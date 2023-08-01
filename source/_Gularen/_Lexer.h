#pragma once

#include <Gularen/Token.h>
#include <vector>

namespace Gularen {
	class Lexer {
	public:
		void set(const std::string& content);

		void tokenize();
		
		const Tokens& get() const;

	private:
		bool check(size_t offset);

		bool is(size_t offset, char c);

		bool isSymbol(size_t offset);

		char get(size_t offset);

		void advance(size_t offset);

		void retreat(size_t offset);

		size_t count(char c);

		void add(TokenType type, const std::string& value);

		void add(TokenType type, const std::string& value, Position begin);

		void add(TokenType type, const std::string& value, Position begin, Position end);

		void addText(const std::string value);

		void tokenizePrefix();

		void tokenizeBlock();
		
		void tokenizeInline();

		void tokenizeCode();

		void tokenizeText();

		void tokenizeSpace();

		void tokenizeTable();

		void tokenizeQuoMark(bool should, TokenType leftType, const std::string& leftValue, TokenType rightType, const std::string& rightValue);

	private:
		size_t index;
		size_t indent = 0;
		Position position;
		std::string content;
		std::basic_string<TokenType> prefix;
		Tokens tokens;
	};
}
