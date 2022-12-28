#pragma once

#include "Token.h"
#include "DS/Array.h"

namespace Gularen
{
    class Lexer
    {
    public:
        void SetBuffer(const String& buffer);
        
        void Parse();

        void PrintDebugInformation();
        
        const String& GetBuffer() const;

        const Array<Token>& GetTokens() const;
        
    private:
        // Main Routine Buffer Parsing

        void ParseNewline();

		void ParseComment();
        
        void ParseFS(TokenType type);

        void ParseQuote(TokenType opening, TokenType ending);
        
        void ParseText();

        // Sub Routine Buffer Parsing

        String ConsumeText();

        // Buffer Iterator Helper

        char GetCurrent();
        
        bool IsInProgress();
        
        void Advance();
        
        void Retreat();
        
        bool IsCurrentText();

        // Token Helper

        void AddToken(Token&& token);

    private:
        String _buffer;
        String::iterator _bufferCursor;

        Array<Token> _tokens;
    };
}
