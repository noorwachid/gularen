#pragma once

#include "Token.hpp"
#include <string>
#include <vector>
#include <stack>

namespace Gularen
{
	class Lexer
	{
	public:
		Lexer();

		void SetBuffer(const std::string& buffer);

		void SetTokens(const std::vector<Token>& tokens);

		void Reset();

		void Parse();

		std::string GetBuffer();

		Token& GetToken(size_t index);

		std::vector<Token>& GetTokens();

		std::string GetTokensAsString();

	private:
		void ParseText();

		void ParseQuotedText();

		void ParseInlineEscapedByte();

		void ParseNewline();

		void ParseFunction();

		void ParseInlineFunction();


	private:
		bool IsValid();

		bool IsValidText();

		bool IsValidSymbol();

		bool IsValidNumeric();

		char GetCurrentByte();

		char GetNextByte(size_t offset = 1);

		void Skip(size_t offset = 1);

		void SkipSpaces();

		void Add(Token&& token);

	private:
		std::vector<Token> mTokens;
		std::string mBuffer;
		size_t mBufferIndex;
		size_t mBufferSize;

		bool mInHeaderLine;
		bool mInLink;
		bool mInCodeBlock;

		size_t mCurrentDepth;
	};
}
