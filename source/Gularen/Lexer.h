#pragma once

#include "Gularen/Token.h"
#include <vector>

namespace Gularen
{
	class Lexer
	{
	public:
		void Set(const std::string& content);

		void Tokenize();

		const Tokens& GetTokens() const;

	private:
		bool CheckBoundary(size_t offset);

		bool IsByte(size_t offset, char c);

		bool IsSymbolByte(size_t offset);

		char GetByte(size_t offset);

		void Advance(size_t offset);

		void Retreat(size_t offset);

		size_t CountRepetendBytes(char c);

		void Add(TokenType type, const std::string& value);

		void AddText(const std::string value);

		void TokenizePrefix();

		void TokenizeBlock();

		void TokenizeInline();

		void TokenizeCode();

		void TokenizeText();

		void TokenizeSpace();

		void TokenizeTable();

		void TokenizeQuoMark(
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
