#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

namespace Gularen {

class Lexer
{
public:
    Lexer();

    void SetBuffer(const std::string& buffer);

    void Reset();
    void Parse();

    std::vector<Token> GetTokens();

    std::string ToString();

private:
    void ParseText();
    void ParseRepeat(char c, TokenType type);
    void ParseNewline();
    void ParseFunction();


private:
    bool IsValid();
    bool IsValidText();
    bool IsValidSymbol();
    bool IsValidNumeric();

    char GetCurrent();
    char GetNext(size_t offset = 1);

    void Skip(size_t offset = 1);
    void SkipSpaces();

    void Add(Token&& token);

private:
    std::vector<Token> tokens;
    std::string buffer;
    size_t bufferIndex;
    size_t bufferSize;

};

}
