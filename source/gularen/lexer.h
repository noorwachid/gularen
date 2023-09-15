#pragma once

#include "gularen/token.h"
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

		void addText(const std::string value);

		void tokenizePrefix();

		void tokenizeBlock();

		void tokenizeInline();

		void tokenizeCode();

		void tokenizeText();

		void tokenizeSpace();

		void tokenizeTable();

		void tokenizeQuoMark(
			bool should, TokenType leftType, const std::string& leftValue, TokenType rightType,
			const std::string& rightValue
		);

	private:
		size_t _index;
		size_t _indent = 0;
		Position _begin;
		Position _end;
		std::string _content;
		std::basic_string<TokenType> _prefix;
		Tokens _tokens;
	};
}
