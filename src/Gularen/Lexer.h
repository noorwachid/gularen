#pragma once

#include "DS/Array.h"
#include "Token.h"

namespace Gularen
{
    class Lexer
    {
      public:
        Array<Token> Tokenize(const String& string);

      private:
        // Main Routine Buffer Parsing

        void ParseNewline();

        void ParseComment();

        void ParseFS(TokenType type);

        void ParseQuote(TokenType opening, TokenType ending);

        void ParseText();

        void ParseSymbol();

        void ParseBlock(bool withAdvancing = false);

        void ParseLine();

        void ParseIndentation();

        void ParseArrow();

        void ParseArrowID();

        void ParseReverseArrowPart();

        void ParseDash();

        void ParseEmojiShortcode();

        // Sub Routine Buffer Parsing

        String ConsumeTextBytes();

        String ConsumeSymbolBytes();

        // Buffer Iterator Helper

        char GetCurrentByte();

        bool IsByteCursorInProgress();

        void AdvanceByteCursor();

        void RetreatByteCursor();

        bool IsCurrentByteText();

        bool IsCurrentByteSymbol();

        // Token Helper

        void AddToken(Token&& token);

        void AddTokenText(const String& text);

      private:
        String _buffer;
        String::iterator _bufferCursor;

        Array<Token> _tokens;

        bool inArrowLine = false;

        UintSize lineCounter = 1;
    };
}
