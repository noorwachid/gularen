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
		bool parseGlobalRules();

		bool parseFormattingRules();

		bool parseNewlineRules();

		bool parseWriter();

		bool parseSymbolOrString();

		bool parseSymbolOrStringOrFormattedString();

		bool parseFunction();

		bool parseFunctionArguments();

		void consumeNewline();

		void consumeQuote(TokenType previousType, TokenType openType, TokenType closeType);

		void consumeBuffer();

		void consumeSymbol();

		void consumeTag(TokenType familyType);

		void consumeString();

		void consumeFormattedString();

		void consumeArray();

		void consumeArguments();

		void consumeBlockKeyword();

	private:
		bool isValid();

		bool isValidBuffer();

		bool isValidString();

		bool isValidFormattedString();

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
		bool inWriter;
		bool inInterpolatedBuffer;
		bool inCodeBlock;

		char previousByte;
		TokenType previousType;

		size_t currentDepth;
	};
}
