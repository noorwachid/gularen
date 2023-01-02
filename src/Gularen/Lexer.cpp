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
        ParseBlock(true);

        while (IsInProgress())
        {
            switch (GetCurrent())
            {
            case '\n':
                ParseNewline();
                Advance();

                ParseBlock();
                break;
			
			case '#':
				ParseComment();
				break;

            case '>':
                ParseArrowID();
                break;
                
            case '-': 
                ParseDash();
                break;

            case ':':
                ParseEmojiShortcode();
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

        AddToken(Token(TokenType::Newline, size));

        Retreat();
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
            if (_tokens.back().type != TokenType::Text)
                AddToken(Token(TokenType::Text, {GetCurrent(), 0x0}));
            else 
                _tokens.back().content += GetCurrent();
    }
    
    void Lexer::ParseSymbol()
    {
        if (IsCurrentSymbol())
            AddToken(Token(TokenType::Symbol, ConsumeSymbol()));
        else
            ParseText();
    }

    void Lexer::ParseBlock(bool withAdvancing)
    {
        _inArrowLine = false;

        switch (GetCurrent()) 
        {
        case '>':
            ParseArrow();
            break;

        case '-':
            ParseLine();
            break;

        default:
            Retreat();
            break;
        }
        
        if (withAdvancing)
            Advance();
    }

    void Lexer::ParseLine()
    {
        Advance();

        if (GetCurrent() == '>')
        {
            Advance();

            if (GetCurrent() == ' ')
            {
                // "-> "

                _inArrowLine = true;
                return AddToken(Token(TokenType::SmallArrow));
            }

            Retreat();
        }

        Retreat();
    }

    void Lexer::ParseArrow()
    {
        Advance();

        if (GetCurrent() == ' ') 
        {
            // "> "
            _inArrowLine = true;
            return AddToken(Token(TokenType::ArrowHead));
        }

        if (GetCurrent() == '-') 
        {
            Advance();

            if (GetCurrent() == '>') 
            {
                Advance();

                if (GetCurrent() == ' ') 
                {
                    // ">-> "
                    _inArrowLine = true;
                    return AddToken(Token(TokenType::Arrow));
                }

                Retreat();
            }

            Retreat();
        }

        if (GetCurrent() == '>') 
        {
            Advance();
            
            if (GetCurrent() == ' ') 
            {
                // ">> "
                _inArrowLine = true;
                return AddToken(Token(TokenType::ArrowTail));
            }

            if (GetCurrent() == '-') 
            {
                Advance();

                if (GetCurrent() == '>') 
                {
                    Advance();

                    if (GetCurrent() == ' ') 
                    {
                        // ">>-> "
                        _inArrowLine = true;
                        return AddToken(Token(TokenType::LargeArrow));
                    }

                    Retreat();
                }

                Retreat();
            }

            if (GetCurrent() == '>') 
            {
                Advance();

                if (GetCurrent() == ' ') 
                {
                    // ">>> "
                    _inArrowLine = true;
                    return AddToken(Token(TokenType::LargeArrowTail));
                }

                if (GetCurrent() == '-') 
                {
                    Advance();

                    if (GetCurrent() == '>') 
                    {
                        Advance();

                        if (GetCurrent() == ' ') 
                        {
                            // ">>>-> "
                            _inArrowLine = true;
                            return AddToken(Token(TokenType::ExtraLargeArrow));
                        }

                        Retreat();
                    }

                    Retreat();
                }

                Retreat();
            }

            Retreat();
        }

        Retreat();
    }

    void Lexer::ParseArrowID()
    {
        if (!_inArrowLine)
            return ParseText();

        AddToken(Token(TokenType::ArrowHead));

        Advance();

        if (GetCurrent() == ' ')
        {
            Advance();
            
            if (IsCurrentSymbol())
            {
                return ParseSymbol();
            }
                
            Retreat();
        }
            
        Retreat();
    }

    void Lexer::ParseDash()
    {
        Advance();

        if (GetCurrent() == '-')
        {
            Advance();

            if (GetCurrent() == '-')
            {
                return AddToken(Token(TokenType::EmDash));
            }

            Retreat();

            return AddToken(Token(TokenType::EnDash));
        }

        Retreat();

        return AddToken(Token(TokenType::Hyphen));
    }

    void Lexer::ParseEmojiShortcode()
    {
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

    String Lexer::ConsumeSymbol()
    {
        String symbol;
        
        while (IsInProgress() && IsCurrentSymbol())
        {
            symbol += GetCurrent();
            
            Advance();
        }
        
        Retreat();
        
        return symbol;
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
            byte == '.'
        ;
    }

    bool Lexer::IsCurrentSymbol()
    {
        char byte = GetCurrent();
        
        return 
            (byte >= 'a' && byte <= 'z') || 
            (byte >= 'A' && byte <= 'Z') ||
            (byte >= '0' && byte <= '9') ||

            byte == '-'
        ;
    }

    // Token Helper

    void Lexer::AddToken(Token&& token)
    {
        _tokens.push_back(token);
    }
}
