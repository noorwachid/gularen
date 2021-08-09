#pragma once

#include "Node.hpp"
#include "Lexer.hpp"
#include <stack>

namespace Gularen {

class AstBuilder
{
public:
    AstBuilder();
    ~AstBuilder();

    void SetBuffer(const std::string& buffer);
    void SetTokens(const std::vector<Token>& tokens);

    void Parse();
    void Reset();
    void DestroyTree();

    std::string GetBuffer();
    std::vector<Token> GetTokens();
    Node* GetTree();

    std::string GetTokensAsString();
    std::string GetTreeAsString();

private:
    void TraverseAndGenerateBuffer(Node* node, size_t depth);
    void TraverseAndDestroyNode(Node* node);

private:
    void ParseNewline(size_t newlineSize = 1);
    void ParseHyphen();
    void ParseLSquareBracket();
    void ParseLAngleBracket();
    void ParseRAngleBracket();
    void ParseRevTail();
    void ParseEqual();
    void ParseLink(NodeType type);

private:
    Node* GetHead();
    void PushHead(Node* node);
    void PopHead();
    bool ShouldPushHead(NodeType type, size_t newlineSize = 0);
    bool ShouldPushValueHead(NodeType type, NodeGroup group, size_t newlineSize = 0);
    bool ShouldPopHead();
    void PairFHead(NodeType type);

    // -- Token operations
    bool IsValid();

    Token& GetCurrentToken();
    Token& GetNextToken(size_t offset = 1);

    void Skip(size_t offset = 1);

    // Buffer definitions
    std::string buffer;

    // Node definitions
    Node* root = nullptr;
    size_t depth = 0;

    std::stack<Node*> headStack;

    // Token definitions
    size_t tokenIndex = 0;
    size_t tokenSize = 0;
    Token emptyToken;

    size_t headerCounter = 0;

    Lexer lexer;
};

}
