#pragma once

#include "Token.h"
#include "DS/Array.h"

namespace Gularen {
    class Lexer {
    public:
        Array<Token> parse(const String& string);

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

        void parseArrow();

        void parseArrowID();

        void parseDash();

        void parseEmojiShortcode();

        // Sub Routine Buffer Parsing

        String consumeTextBytes();

        String consumeSymbolBytes();

        // Buffer Iterator Helper

        char getCurrentByte();
        
        bool isByteCursorInProgress();
        
        void advanceByteCursor();
        
        void retreatByteCursor();
        
        bool isCurrentByteText();

        bool isCurrentByteSymbol();

        // Token Helper

        void addToken(Token&& token);

    private:
        String _buffer;
        String::iterator _bufferCursor;

        Array<Token> _tokens;
        
        bool _inArrowLine = false;
    };
}
