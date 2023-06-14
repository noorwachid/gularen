#include "Lexer.h"
#include <iostream>

namespace Gularen
{
    // PUBLIC DEFINITIONS

    Array<Token> Lexer::Tokenize(const String& newBuffer)
    {
        lineCounter = 1;

        buffer = newBuffer;
        bufferCursor = buffer.begin();

        tokens.clear();

        // Guarantied to have one token in the collection
        AddToken(Token(TokenType::BODocument));
        ParseBlock(true);

        while (IsByteCursorInProgress())
        {
            switch (GetCurrentByte())
            {
            case '\n':
                ParseNewline();
                AdvanceByteCursor();

                ParseBlock();
                break;

            case '#':
                ParseComment();
                break;

            case '>':
                ParseArrowID();
                break;

            case '<':
                ParseReverseArrowPart();
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

            AdvanceByteCursor();
        }

        // Guarantied to have two token in the collection
        AddToken(Token(TokenType::EODocument));

        // TODO: Optimize this copy
        return tokens;
    }

    // PRIVATE DEFINITIONS

    // Main Routine Buffer Parsing

    void Lexer::ParseNewline()
    {
        UintSize size = 0;

        while (IsByteCursorInProgress() && GetCurrentByte() == '\n')
        {
            AdvanceByteCursor();
            ++size;
        }

        if (tokens.back().type != TokenType::Newline)
            AddToken(Token(TokenType::Newline, size));
        else
            tokens.back().size += size;

        lineCounter += size;

        RetreatByteCursor();
    }

    void Lexer::ParseComment()
    {
        AdvanceByteCursor();

        while (IsByteCursorInProgress() && GetCurrentByte() != '\n')
            AdvanceByteCursor();

        if (GetCurrentByte() == '\n' || GetCurrentByte() == '\0')
            RetreatByteCursor();
    }

    void Lexer::ParseFS(TokenType type) { AddToken(Token(type)); }

    void Lexer::ParseQuote(TokenType opening, TokenType ending)
    {
        auto& previous = tokens.back();

        if ((previous.type == TokenType::Text && previous.content.back() == ' ') ||
            previous.type == TokenType::Newline || previous.type == TokenType::BODocument)
            return AddToken(Token(opening));

        AddToken(Token(ending));
    }

    void Lexer::ParseText()
    {
        if (IsCurrentByteText())
            AddToken(Token(TokenType::Text, ConsumeTextBytes()));
        else if (tokens.back().type != TokenType::Text)
            AddToken(Token(TokenType::Text, {GetCurrentByte(), 0x0}));
        else
            tokens.back().content += GetCurrentByte();
    }

    void Lexer::ParseSymbol()
    {
        if (IsCurrentByteSymbol())
            AddToken(Token(TokenType::Symbol, ConsumeSymbolBytes()));
        else
            ParseText();
    }

    void Lexer::ParseBlock(bool withAdvancing)
    {
        inArrowLine = false;

        if (GetCurrentByte() == ' ')
        {
            ParseIndentation();

            if (tokens.back().type == TokenType::Indentation && GetCurrentByte() == '\n')
            {
                tokens.pop_back();
                ParseNewline();
            }
        }

        switch (GetCurrentByte())
        {
        case '>':
            ParseArrow();
            break;

        case '-':
            ParseLine();
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            ParseList();
            break;

        default:
            RetreatByteCursor();
            break;
        }

        if (withAdvancing)
            AdvanceByteCursor();
    }

    void Lexer::ParseLine()
    {
        AdvanceByteCursor();

        if (GetCurrentByte() == '>')
        {
            AdvanceByteCursor();

            if (GetCurrentByte() == ' ')
            {
                // "-> "

                inArrowLine = true;
                return AddToken(Token(TokenType::SArrow));
            }

            RetreatByteCursor();
        }

        RetreatByteCursor();
    }

    void Lexer::ParseIndentation()
    {
        UintSize size = 0;

        while (IsByteCursorInProgress() && GetCurrentByte() == ' ')
        {
            AdvanceByteCursor();

            ++size;
        }

        UintSize indentSpaceSize = 4;
        UintSize indentLevel = size / indentSpaceSize;
        UintSize indentReminder = size % indentSpaceSize;

        if (indentLevel)
            AddToken(Token(TokenType::Indentation, indentLevel));

        if (indentReminder)
        {
            for (UintSize leftOverIndex = 0; leftOverIndex < indentReminder; ++leftOverIndex)
                RetreatByteCursor();
        }
    }

    void Lexer::ParseArrow()
    {
        AdvanceByteCursor();

        if (GetCurrentByte() == ' ')
        {
            // "> "
            inArrowLine = true;
            return AddToken(Token(TokenType::ArrowHead));
        }

        if (GetCurrentByte() == '-')
        {
            AdvanceByteCursor();

            if (GetCurrentByte() == '>')
            {
                AdvanceByteCursor();

                if (GetCurrentByte() == ' ')
                {
                    // ">-> "
                    inArrowLine = true;
                    return AddToken(Token(TokenType::Arrow));
                }

                RetreatByteCursor();
            }

            RetreatByteCursor();
        }

        if (GetCurrentByte() == '>')
        {
            AdvanceByteCursor();

            if (GetCurrentByte() == ' ')
            {
                // ">> "
                inArrowLine = true;
                return AddToken(Token(TokenType::ArrowTail));
            }

            if (GetCurrentByte() == '-')
            {
                AdvanceByteCursor();

                if (GetCurrentByte() == '>')
                {
                    AdvanceByteCursor();

                    if (GetCurrentByte() == ' ')
                    {
                        // ">>-> "
                        inArrowLine = true;
                        return AddToken(Token(TokenType::LArrow));
                    }

                    RetreatByteCursor();
                }

                RetreatByteCursor();
            }

            if (GetCurrentByte() == '>')
            {
                AdvanceByteCursor();

                if (GetCurrentByte() == ' ')
                {
                    // ">>> "
                    inArrowLine = true;
                    return AddToken(Token(TokenType::LArrowTail));
                }

                if (GetCurrentByte() == '-')
                {
                    AdvanceByteCursor();

                    if (GetCurrentByte() == '>')
                    {
                        AdvanceByteCursor();

                        if (GetCurrentByte() == ' ')
                        {
                            // ">>>-> "
                            inArrowLine = true;
                            return AddToken(Token(TokenType::XLArrow));
                        }

                        RetreatByteCursor();
                    }

                    RetreatByteCursor();
                }

                RetreatByteCursor();
            }

            RetreatByteCursor();
        }

        RetreatByteCursor();
    }

    void Lexer::ParseArrowID()
    {
        if (!inArrowLine)
            return ParseText();

        AddToken(Token(TokenType::ArrowHead));

        AdvanceByteCursor();

        if (GetCurrentByte() == ' ')
        {
            AdvanceByteCursor();

            if (IsCurrentByteSymbol())
            {
                return ParseSymbol();
            }

            RetreatByteCursor();
        }

        RetreatByteCursor();
    }

    void Lexer::ParseReverseArrowPart()
    {
        String buffer;

        while (IsByteCursorInProgress() && GetCurrentByte() == '<')
        {
            buffer += '<';
            AdvanceByteCursor();
        }

        RetreatByteCursor();

        switch (buffer.size())
        {
        case 1:
            AddToken(TokenType::RArrowHead);
            break;

        case 2:
            AddToken(TokenType::RArrowTail);
            break;

        case 3:
            AddToken(TokenType::RLArrowTail);
            break;

        default:
            AddTokenText(buffer);
            break;
        }
    }

    void Lexer::ParseDash()
    {
        AdvanceByteCursor();

        if (GetCurrentByte() == '-')
        {
            AdvanceByteCursor();

            if (GetCurrentByte() == '-')
            {
                return AddToken(Token(TokenType::EmDash));
            }

            RetreatByteCursor();

            return AddToken(Token(TokenType::EnDash));
        }

        RetreatByteCursor();

        return AddToken(Token(TokenType::Hyphen));
    }

    void Lexer::ParseList() 
    {
        String number = ConsumeNumericBytes();

        if (!number.empty()) 
        {
            AdvanceByteCursor();

            if (GetCurrentByte() == '.')
            {
                AddToken(Token(TokenType::NumericBullet, number));
                AdvanceByteCursor();
                return;
            }

            RetreatByteCursor();
        }
    }

    void Lexer::ParseEmojiShortcode() {}

    // Sub Routine Buffer Parsing
    
    String Lexer::ConsumeNumericBytes()
    {
        String text;

        while (IsByteCursorInProgress() && IsCurrentByteNumeric())
        {
            text += GetCurrentByte();

            AdvanceByteCursor();
        }

        RetreatByteCursor();

        return text;
    }

    String Lexer::ConsumeTextBytes()
    {
        String text;

        while (IsByteCursorInProgress() && IsCurrentByteText())
        {
            text += GetCurrentByte();

            AdvanceByteCursor();
        }

        RetreatByteCursor();

        return text;
    }

    String Lexer::ConsumeSymbolBytes()
    {
        String symbol;

        while (IsByteCursorInProgress() && IsCurrentByteSymbol())
        {
            symbol += GetCurrentByte();

            AdvanceByteCursor();
        }

        RetreatByteCursor();

        return symbol;
    }

    // Buffer Iterator Helper

    char Lexer::GetCurrentByte() { return *bufferCursor; }

    bool Lexer::IsByteCursorInProgress() { return bufferCursor < buffer.end(); }

    void Lexer::AdvanceByteCursor() { ++bufferCursor; }

    void Lexer::RetreatByteCursor() { --bufferCursor; }

    bool Lexer::IsCurrentByteNumeric()
    {
        char byte = GetCurrentByte();

        return (byte >= '0' && byte <= '9');
    }

    bool Lexer::IsCurrentByteText()
    {
        char byte = GetCurrentByte();

        return (byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') || (byte >= '0' && byte <= '9') ||

               byte == ' ' || byte == ',' || byte == '.';
    }

    bool Lexer::IsCurrentByteSymbol()
    {
        char byte = GetCurrentByte();

        return (byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') || (byte >= '0' && byte <= '9') ||

               byte == '-';
    }

    // Token Helper

    void Lexer::AddToken(Token&& token)
    {
        token.line = lineCounter;
        tokens.push_back(token);
    }

    void Lexer::AddTokenText(const String& text)
    {
        if (tokens.back().type == TokenType::Text)
            tokens.back().content += text;
        else
            AddToken(Token(TokenType::Text, text));
    }
}
