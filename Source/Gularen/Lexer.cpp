#include "Lexer.hpp"
#include "IO.hpp"

namespace Gularen {

Lexer::Lexer()
{
}

void Lexer::SetBuffer(const std::string& buffer)
{
    this->buffer = buffer;
    bufferSize = buffer.size();

    Reset();
}

void Lexer::Parse()
{
    Reset();
    Add(Token(TokenType::DocumentBegin));
    ParseNewline();

    while (IsValid())
    {
        // Main switchboard
        switch (GetCurrent())
        {
            case '*':
                Add(Token(TokenType::Asterisk));
                Skip();
                break;

            case '_':
                Add(Token(TokenType::Underline));
                Skip();
                break;

            case '`':
                Add(Token(TokenType::Backtick));
                Skip();
                break;

            case '\n':
            {
                size_t size = 0;
                while (IsValid() && GetCurrent() == '\n')
                {
                    ++size;
                    Skip();
                }

                if (!tokens.empty() && tokens.back().type == TokenType::Newline)
                    tokens.back().size += size;
                else
                {
                    Token token(TokenType::Newline);
                    token.size = size;
                    Add(std::move(token));
                }

                ParseNewline();
                break;
            }

            case '^':
                while (IsValid() && GetCurrent() != '\n')
                    Skip();
                break;

            case '<':
                Add(Token(TokenType::RevHead));
                Skip();
                break;

            default:
                if (IsValidText())
                    ParseText();
                else
                    Skip(); // Unhandled symbols
                break;
        }
    }

    Add(Token(TokenType::DocumentEnd));
}

void Lexer::Reset()
{
    bufferIndex = 0;
    tokens.clear();
}

std::vector<Token> Lexer::GetTokens()
{
    return tokens;
}

std::string Lexer::ToString()
{
    std::string buffer;

    for (Token& token: tokens)
        buffer += token.ToString() + "\n";

    return buffer;
}

void Lexer::ParseText()
{
    Token token;
    token.type = TokenType::Text;

    while (IsValid() && IsValidText())
    {
        token.value += GetCurrent();
        Skip();
    }

    Add(std::move(token));
}

void Lexer::ParseRepeat(char c, TokenType type)
{
    Token token(type);
    token.size = 1;
    Skip();

    while (IsValid() && GetCurrent() == c)
    {
        ++token.size;
        Skip();
    }

    Add(std::move(token));
}

void Lexer::ParseNewline()
{
    switch (GetCurrent())
    {
        case ' ':
            ParseRepeat(' ', TokenType::Space);
            ParseNewline();
            break;

        case '-':
        {
            size_t size = 0;

            while (IsValid() && GetCurrent() == '-')
            {
                Skip();
                ++size;
            }

            if (size == 1)
            {
                if (GetCurrent() == '>' && GetNext() == ' ')
                {
                    Add(Token(TokenType::Arrow));
                    Skip();
                    SkipSpaces();
                }
                else
                {
                    Add(Token(TokenType::Bullet));
                    SkipSpaces();
                }
            }
            else
                Add(Token(TokenType::Line));
            break;
        }

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
        {
            std::string buffer;

            while (IsValid() && IsValidNumeric())
            {
                buffer += GetCurrent();
                Skip();
            }

            if (GetCurrent() == '.' && GetNext() == ' ')
            {
                Add(Token(TokenType::NBullet));
                Skip(2);
            }
            else
                Add(Token(TokenType::Text, buffer));

            break;
        }

        case '.':
            if (GetNext(1) == '.' && GetNext(2) == ' ')
            {
                Add(Token(TokenType::NBullet));
                Skip(3);
            }
            break;

        case '[':
            if ((GetNext(1) == ' ' || GetNext(1) == '+') && GetNext(2) == ']')
            {
                Token token(TokenType::CheckBox);
                token.size = GetCurrent() == '+' ? 1 : 0;

                Add(std::move(token));
                Skip(3);
                SkipSpaces();
            }
            break;

        case '>':
        {
            Skip();
            if (GetCurrent() == ' ')
            {
                Add(Token(TokenType::Head));
                SkipSpaces();
                break;
            }
            else if (GetCurrent() == '>')
            {
                Skip();
                if (GetCurrent() == ' ')
                {
                    Add(Token(TokenType::Tail));
                    SkipSpaces();
                }
                else if (GetCurrent() == '>' && GetNext() == ' ')
                {
                    Add(Token(TokenType::BigTail));
                    Skip();
                    SkipSpaces();
                }
                else if (GetCurrent() == '-' && GetNext(1) == '>' && GetNext(2) == ' ')
                {
                    Add(Token(TokenType::BigArrow));
                    Skip(2);
                    SkipSpaces();
                }
            }
            else if (GetCurrent() == '-')
            {
                Skip();
                if (GetCurrent() == '>' && GetNext() == ' ')
                {
                    Add(Token(TokenType::MediumArrow));
                    Skip();
                    SkipSpaces();
                }
            }
        }

        case '<':
            if (GetNext() == '<')
            {
                Skip(2);
                if (GetCurrent() == '<')
                {
                    Add(Token(TokenType::RevBigTail));
                    Skip();
                }
                else
                    Add(Token(TokenType::RevTail));
            }
            break;

        case ':':
            if (GetNext() == ':')
            {
                Add(Token(TokenType::Box));
                Skip(2);
                SkipSpaces();
                ParseFunction();
            }
            break;
    }
}

void Lexer::ParseFunction()
{
    std::string symbol;

    while (IsValid() && IsValidSymbol())
    {
        symbol += GetCurrent();
        Skip();
    }
    Add(Token(TokenType::Symbol));
    SkipSpaces();
}

bool Lexer::IsValid()
{
    return bufferIndex < bufferSize;
}

bool Lexer::IsValidText()
{
    char c = GetCurrent();

    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
            c == ' ' || c == '-' ||
            c == ',' || c == '.' ||
            c == '!' || c == '?' ||
            c == '\'' || c == '"' ||
            c == ';' || c == ':';
}

bool Lexer::IsValidSymbol()
{
    char c = GetCurrent();

    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

bool Lexer::IsValidNumeric()
{
    char c = GetCurrent();

    return c >= '0' && c <= '9';
}

char Lexer::GetCurrent()
{
    return buffer[bufferIndex];
}

char Lexer::GetNext(size_t offset)
{
    return bufferIndex + offset < bufferSize ? buffer[bufferIndex + offset] : 0;
    }

    void Lexer::Skip(size_t offset)
    {
    bufferIndex += offset;
}

void Lexer::SkipSpaces()
{
    while (IsValid() && GetCurrent() == ' ')
        Skip();
}

void Lexer::Add(Token&& token)
{
    tokens.emplace_back(token);
}

} // namespace Nagasari
