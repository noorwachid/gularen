#include "AstBuilder.hpp"
#include "IO.hpp"

namespace Gularen {

AstBuilder::AstBuilder()
{
}

void AstBuilder::SetTokens(std::vector<Token> tokens)
{
    this->tokens = tokens;
    tokenIndex = 0;
    tokenSize = tokens.size();

    if (root)
        delete root;

    root = new Node(NodeType::Root);

    while (!headStack.empty())
        headStack.pop();

    headStack.push(root);
}

void AstBuilder::Parse()
{
    if (GetCurrent().type == TokenType::DocumentBegin)
        Skip();

    ParseNewline();

    while (IsValid())
    {
        switch (GetCurrent().type)
        {
            case TokenType::Text:
                GetHead()->Add(new ValueNode(NodeType::Text, GetCurrent().value));
                Skip();
                break;

            case TokenType::Asterisk:
                PairFHead(NodeType::FBold);
                break;

            case TokenType::Underline:
                PairFHead(NodeType::FItalic);
                break;

            case TokenType::Backtick:
                PairFHead(NodeType::FMonospace);
                break;

            case TokenType::Newline:
            {
                size_t newlineSize = GetCurrent().size;
                // GetHead()->Add(new SizeNode(NodeType::Newline, GetCurrent().size));
                Skip();
                ParseNewline(newlineSize);
                break;
            }

            case TokenType::Anchor:
            {
                if (GetHead()->group == NodeGroup::Header)
                {
                    static_cast<ValueNode*>(GetHead())->value = GetCurrent().value;
                }
                Skip();
                break;
            }

            case TokenType::RevTail:
                GetHead()->Add(new Node(NodeType::LineBreak, NodeGroup::Break));
                Skip();
                break;

            case TokenType::Teeth:
            {
                Skip();
                if (GetCurrent().type == TokenType::Text && GetNext().type == TokenType::Teeth)
                {
                    GetHead()->Add(new ValueNode(NodeType::InlineCode, GetCurrent().value));
                    Skip(2);
                }
                break;
            }

            default:
                Skip();
                break;
        }
    }

    ParseNewline();
}

Node* AstBuilder::GetTree()
{
    return root;
}

std::string AstBuilder::ToString()
{
    std::string buffer;

    TraverseToString(GetTree(), 0, buffer);

    return buffer;
}

void Gularen::AstBuilder::TraverseToString(Node* node, size_t depth, std::string& buffer)
{
    for (size_t i = 0; i < depth; ++i)
        buffer += "    ";

    buffer += node->ToString() + "\n";

    for (Node* child: node->children)
        TraverseToString(child, depth + 1, buffer);
}

void AstBuilder::ParseNewline(size_t newlineSize)
{
    // --- BEGIN IDENTATIONS ---

    size_t currentDepth = 0;

    if (GetCurrent().type == TokenType::Space)
    {
        currentDepth = GetCurrent().size / 4;
        Skip();
    }

    if (currentDepth > depth)
    {
        size_t distance = currentDepth - depth;
        for (size_t i = 0; i < distance; ++i)
            PushHead(new Node(NodeType::Indent));
    }
    else if (currentDepth < depth)
    {
        size_t distance = depth - currentDepth;
        size_t i = 0;
        while (i < distance)
        {
            if (GetHead()->type == NodeType::Root)
                break;

            if (GetHead()->type == NodeType::Indent)
                ++i;

            PopHead();
        }
    }

    depth = currentDepth;

    // --- END INDENTATIONS ---

    // --- POP ITEM NODE HEAD ---
    if (GetHead()->group == NodeGroup::Item)
        PopHead();

    switch (GetCurrent().type)
    {
        case TokenType::Text:
            if (!ShouldPushHead(NodeType::Paragraph, newlineSize))
                GetHead()->Add(new Node(NodeType::Newline));
            break;

        case TokenType::Tail:
            switch (GetCurrent().size)
            {
                case 1:
                    ShouldPushValueHead(NodeType::Minisection, NodeGroup::Header);
                    break;
                case 2:
                    ShouldPushValueHead(NodeType::Part, NodeGroup::Header);
                    break;
                case 3:
                    ShouldPushValueHead(NodeType::Title, NodeGroup::Header);
                    break;

                default:
                    break;
            }
            Skip();
            break;

        case TokenType::Arrow:
            switch (GetCurrent().size)
            {
                case 1:
                    ShouldPushValueHead(NodeType::Subsubsection, NodeGroup::Header);
                    break;
                case 2:
                    ShouldPushValueHead(NodeType::Subsection, NodeGroup::Header);
                    break;
                case 3:
                    ShouldPushValueHead(NodeType::Section, NodeGroup::Header);
                    break;
                case 4:
                    ShouldPushValueHead(NodeType::Chapter, NodeGroup::Header);
                    break;

                default:
                    break;
            }
            Skip();
            break;

        case TokenType::RevTail:
            ParseRevTail();
            break;

        case TokenType::Bullet:
            ShouldPushHead(NodeType::List);
            PushHead(new Node(NodeType::Item, NodeGroup::Item));
            Skip();
            break;

        case TokenType::NBullet:
            ShouldPushHead(NodeType::NList);
            PushHead(new Node(NodeType::Item, NodeGroup::Item));
            Skip();
            break;

        case TokenType::CheckBox:
            ShouldPushHead(NodeType::CheckList);
            PushHead(new BooleanNode(NodeType::CheckItem, NodeGroup::Item, GetCurrent().size));
            Skip();
            break;

        default:
            break;
    }
}

void AstBuilder::ParseHyphen()
{
}

void AstBuilder::ParseLSquareBracket()
{
}

void AstBuilder::ParseLAngleBracket()
{
}

void Gularen::AstBuilder::ParseRAngleBracket()
{
}

void AstBuilder::ParseRevTail()
{
    switch (GetCurrent().size)
    {
        case 1: GetHead()->Add(new Node(NodeType::LineBreak, NodeGroup::Break)); break;
        case 2: GetHead()->Add(new Node(NodeType::ThematicBreak, NodeGroup::Break)); break;
        case 3: GetHead()->Add(new Node(NodeType::PageBreak, NodeGroup::Break)); break;
    }
    Skip();
}

void AstBuilder::ParseEqual()
{
}

Node* AstBuilder::GetHead()
{
    return headStack.top();
}

void AstBuilder::PushHead(Node* node)
{
    GetHead()->Add(node);
    headStack.push(node);
}

void AstBuilder::PopHead()
{
    if (headStack.size() > 1)
        headStack.pop();
}

bool AstBuilder::ShouldPushHead(NodeType type, size_t newlineSize)
{
    if (GetHead()->type != type || newlineSize > 1)
    {
        ShouldPopHead();
        PushHead(new Node(type));

        return true;
    }
    return false;
}

bool AstBuilder::ShouldPushValueHead(NodeType type, NodeGroup group, size_t newlineSize)
{
    if (GetHead()->type != type || newlineSize > 1)
    {
        ShouldPopHead();
        PushHead(new ValueNode(type, group));

        return true;
    }
    return false;
}

bool AstBuilder::ShouldPopHead()
{
    if (GetHead()->type != NodeType::Indent)
    {
        PopHead();
        return true;
    }
    return false;
}

void AstBuilder::PairFHead(NodeType type)
{
    if (GetHead()->type == type)
        PopHead();
    else
        PushHead(new Node(type));

    Skip();
}

bool AstBuilder::IsValid()
{
    return tokenIndex < tokenSize;
}

Token& AstBuilder::GetCurrent()
{
    return tokens[tokenIndex];
}

Token& AstBuilder::GetNext(size_t offset)
{
    return tokenIndex + offset < tokenSize ? tokens[tokenIndex + offset] : emptyToken;
}

void AstBuilder::Skip(size_t offset)
{
    tokenIndex += offset;
}

}
