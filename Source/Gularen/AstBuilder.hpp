#pragma once

#include "Node.hpp"
#include "Token.hpp"
#include <stack>

namespace Gularen {

class AstBuilder
{
public:
    AstBuilder();

    void SetTokens(std::vector<Token> tokens);

    void Parse();

    Node* GetTree();

    std::string ToString();

private:
    void TraverseToString(Node* node, size_t depth, std::string& buffer);

private:
    void ParseNewline(size_t newlineSize = 1);
    void ParseHyphen();
    void ParseLSquareBracket();
    void ParseLAngleBracket();
    void ParseRAngleBracket();
    void ParseRevTail();
    void ParseEqual();

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

    Token& GetCurrent();
    Token& GetNext(size_t offset = 1);

    void Skip(size_t offset = 1);

    // Node definitions
    Node* root = nullptr;
    size_t depth = 0;

    std::stack<Node*> headStack;

    // Token definitions
    size_t tokenIndex = 0;
    size_t tokenSize = 0;
    Token emptyToken;

    size_t headerCounter = 0;

    std::vector<Token> tokens;
};

}
