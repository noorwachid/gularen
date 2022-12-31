#include "Lexer.h"
#include <iostream>

namespace Gularen 
{
    // PUBLIC DEFINITIONS
    
    Array<Token> Lexer::Parse(const String& buffer)
    {
        _buffer = buffer;
        _bufferCursor = _buffer.begin();

        _tokens.clear();

        // Guarantied to have one token in the collection
        AddToken(Token(TokenType::BODocument));

        while (IsInProgress())
        {
            switch (GetCurrent())
            {
            case '\n':
                ParseNewline();
                break;
			
			case '#':
				ParseComment();
				break;

            case '*':
                ParseFS(TokenType::Asterisk);
                break;
                
            case '_':
                ParseFS(TokenType::Underscore);
                break;
                
            case '`':
                ParseFS(TokenType::Backtick);
                break;
                
            case '\'':
                ParseQuote(TokenType::LSQuote, TokenType::RSQuote);
                break;

            case '"':
                ParseQuote(TokenType::LDQuote, TokenType::RDQuote);
                break;
                
            default:
                ParseText();
                break;
            }
            
            Advance();
        }

        // Guarantied to have two token in the collection
        AddToken(Token(TokenType::EODocument));

        // TODO: Optimize this copy
        return _tokens;
    }

    // PRIVATE DEFINITIONS

    // Main Routine Buffer Parsing

    void Lexer::ParseNewline()
    {
        UintSize size = 0;

        while (IsInProgress() && GetCurrent() == '\n')
        {
            Advance();
            ++size;
        }

        Retreat();
        AddToken(Token(TokenType::Newline, size));
    }

	void Lexer::ParseComment()
	{
		Advance();

		while (IsInProgress() && GetCurrent() != '\n')
			Advance();

		if (GetCurrent() == '\n' || GetCurrent() == '\0') 
			Retreat();
	}
    
    void Lexer::ParseFS(TokenType type)
    {
        AddToken(Token(type));
    }  

    void Lexer::ParseQuote(TokenType opening, TokenType ending)
    {
        auto& previous = _tokens.back();

        if ((previous.type == TokenType::Text && previous.content.back() == ' ') || 
            previous.type == TokenType::Newline || 
            previous.type == TokenType::BODocument)
            return AddToken(Token(opening));

        AddToken(Token(ending));
    }
    
    void Lexer::ParseText()
    {
        if (IsCurrentText())
            AddToken(Token(TokenType::Text, ConsumeText()));
        else
            // TODO: check if previously is Text then append
            AddToken(Token(TokenType::Text, {GetCurrent(), 0x0}));
    }

    // Sub Routine Buffer Parsing
    
    String Lexer::ConsumeText()
    {
        String text;
        
        while (IsInProgress() && IsCurrentText())
        {
            text += GetCurrent();
            
            Advance();
        }
        
        Retreat();
        
        return text;
    }

    // Buffer Iterator Helper

    char Lexer::GetCurrent()
    {
        return *_bufferCursor;
    }
    
    bool Lexer::IsInProgress()
    {
        return _bufferCursor < _buffer.end();
    }
    
    void Lexer::Advance()
    {
        ++_bufferCursor;
    }
    
    void Lexer::Retreat()
    {
        --_bufferCursor;
    }
    
    bool Lexer::IsCurrentText()
    {
        char byte = GetCurrent();

        return 
            (byte >= 'a' && byte <= 'z') || 
            (byte >= 'A' && byte <= 'Z') ||
            (byte >= '0' && byte <= '9') ||

            byte == ' ' || 
            byte == ',' ||
            byte == '.' ||
            byte == '!' ||
            byte == '?' ||
            byte == ':' ||
            byte == '-'
        ;
    }

    // Token Helper

    void Lexer::AddToken(Token&& token)
    {
        _tokens.push_back(token);
    }
}
