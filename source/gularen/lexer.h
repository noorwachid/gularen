#pragma once

#include "gularen/token.h"
#include <vector>

namespace Gularen
{
	class Lexer
	{
	public:
		void Set(const std::string& content);

		void Tokenize();

		const Tokens& Get() const;

	private:
		bool Check(size_t offset);

		bool Is(size_t offset, char c);

		bool IsSymbol(size_t offset);

		char Get(size_t offset);

		void Advance(size_t offset);

		void Retreat(size_t offset);

		size_t Count(char c);

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
