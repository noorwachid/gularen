#pragma once

#include "Token.h"
#include "DS/Array.h"

namespace Gularen
{
    class Lexer
    {
    public:
        Array<Token> Parse(const String& string);

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

        void ParseArrow();

        void ParseArrowID();

        void ParseDash();

        void ParseEmojiShortcode();

        // Sub Routine Buffer Parsing

        String ConsumeText();

        String ConsumeSymbol();

        // Buffer Iterator Helper

        char GetCurrent();
        
        bool IsInProgress();
        
        void Advance();
        
        void Retreat();
        
        bool IsCurrentText();

        bool IsCurrentSymbol();

        // Token Helper

        void AddToken(Token&& token);

    private:
        String _buffer;
        String::iterator _bufferCursor;

        Array<Token> _tokens;
        
        bool _inArrowLine = false;
    };
}
