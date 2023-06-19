#pragma once

#include "DS/Array.h"
#include "Token.h"

namespace Gularen {
	class Lexer {
	public:
		Array<Token> tokenize(const String& string);

	private:
		// Main Routine Buffer Parsing

		void parseNewline();

		void parseComment();

		void parseFS(TokenType type);

		void parseQuote(TokenType opening, TokenType ending);

		void parseText();

		void parseSymbol();

		void parseBlock(bool withAdvancing = false);

		void parseLine();

		void parseIndentation();

		void parseArrow();

		void parseArrowID();

		void parseReverseArrowPart();

		void parseDash();

		void parseList();

		void parseEmojiShortcode();

		// Sub Routine Buffer Parsing
		//
		String consumeNumericBytes();

		String consumeTextBytes();

		String consumeSymbolBytes();

		// Buffer Iterator Helper

		char getCurrentByte();

		bool isByteCursorInProgress();

		void advanceByteCursor();

		void retreatByteCursor();

		bool isCurrentByteNumeric();

		bool isCurrentByteText();

		bool isCurrentByteSymbol();

		// Token Helper

		void addToken(Token&& token);

		void addTokenText(const String& text);

	private:
		String buffer;
		String::iterator bufferCursor;

		Array<Token> tokens;

		bool inArrowLine = false;

		UintSize lineCounter = 1;
	};
}
