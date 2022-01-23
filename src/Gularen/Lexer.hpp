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

		void setBuffer(const std::string& buffer);

		void setTokens(const std::vector<Token>& tokens);

		void reset();

		void parse();

		std::string getBuffer();

		Token& getTokenAt(size_t index);

		std::vector<Token>& getTokens();

		std::string getTokensAsString();

	private:
		void parseBuffer();

		void parseQuoteBuffer(TokenType previousType, TokenType openType, TokenType closeType);

		void parseString();

		void parseInterpolatedString();

		void parseNewline();

		void parseFunction();

		void parseInlineFunction();

	private:
		bool isValid();

		bool isValidBuffer();

		bool isValidSymbol();

		bool isValidNumeric();

		char getCurrentByte();

		char getPreviousByte();

		char getNextByte(size_t offset = 1);

		void skip(size_t offset = 1);

		void skipSpaces();

		void add(Token&& token);

	private:
		std::vector<Token> tokens;
		std::string buffer;
		size_t bufferIndex;
		size_t bufferSize;

		bool inHeaderLine;
		bool inInterpolatedBuffer;
		bool inCodeBlock;

		char previousByte;
		TokenType previousType;

		size_t currentDepth;
	};
}
