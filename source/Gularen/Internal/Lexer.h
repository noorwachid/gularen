#pragma once

#include "Gularen/Internal/Token.h"
#include <vector>

namespace Gularen {
	class Lexer {
	public:
		void set(const std::string& content);

		void parse();

		const Tokens& getTokens() const;

	private:
		bool checkBoundary(size_t offset);

		bool isByte(size_t offset, char c);

		bool isSymbolByte(size_t offset);

		char getByte(size_t offset);

		void advance(size_t offset);

		void retreat(size_t offset);

		size_t countRepetendBytes(char c);

		void add(TokenType type, const std::string& value);

		void addText(const std::string value);

		void parsePrefix();

		void parseBlock();

		void parseInline();

		void parseCode();

		void parseText();

		void parseSpace();

		void parseTable();

		void parseQuoMark(
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
