#include "AstBuilder.hpp"
#include "IO.hpp"

namespace Gularen {

AstBuilder::AstBuilder()
{
}

AstBuilder::~AstBuilder()
{
    if (root)
        DestroyTree();
}

void AstBuilder::SetBuffer(const std::string &buffer)
{
    lexer.SetBuffer(buffer);
    lexer.Parse();

    Reset();
}

void AstBuilder::SetTokens(const std::vector<Token> &tokens)
{
    lexer.SetTokens(tokens);

    Reset();
}

void AstBuilder::Parse()
{
    if (GetCurrentToken().type == TokenType::DocumentBegin)
        Skip();

    ParseNewline();

    while (IsValid())
    {
        switch (GetCurrentToken().type)
        {
            case TokenType::Text:
                GetHead()->Add(new ValueNode(NodeType::Text, GetCurrentToken().value));
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
                size_t newlineSize = GetCurrentToken().size;
                // GetHead()->Add(new SizeNode(NodeType::Newline, GetCurrent().size));
                Skip();
                ParseNewline(newlineSize);
                break;
            }

            case TokenType::Anchor:
            {
                if (GetHead()->group == NodeGroup::Header)
                {
                    static_cast<ValueNode*>(GetHead())->value = GetCurrentToken().value;
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
                if (GetCurrentToken().type == TokenType::Text && GetNextToken().type == TokenType::Teeth)
                {
                    GetHead()->Add(new ValueNode(NodeType::InlineCode, GetCurrentToken().value));
                    Skip(2);
                }
                break;
            }

            case TokenType::LCurlyBracket:
                Skip();
                switch (GetCurrentToken().type)
                {
                    case TokenType::Symbol:
                        GetHead()->Add(new ValueNode(NodeType::Curtain, GetCurrentToken().value));
                        Skip();
                        break;

                    case TokenType::Colon:
                        ParseLink(NodeType::Link);
                        break;

                    case TokenType::ExclamationMark:
                        ParseLink(NodeType::LocalLink);
                        break;

                    case TokenType::QuestionMark:
                        ParseLink(NodeType::InlineImage);
                        break;

                    default:
                        break;
                }

                break;

            case TokenType::RCurlyBracket:
                Skip();
                PopHead();
                break;

            case TokenType::Pipe:
                while (!headStack.empty() && headStack.top()->type != NodeType::TableRow)
                    PopHead();
                PushHead(new Node(NodeType::TableColumn, NodeGroup::Table));

            default:
                Skip();
                break;
        }
    }

    ParseNewline();
}

void AstBuilder::Reset()
{
    tokenIndex = 0;
    tokenSize = lexer.GetTokens().size();

    if (root)
        delete root;

    root = new Node(NodeType::Root);

    while (!headStack.empty())
        headStack.pop();

    while (!blockStack.empty())
        blockStack.pop();

    headStack.push(root);
}

void AstBuilder::DestroyTree()
{
    TraverseAndDestroyNode(root);
}

std::string AstBuilder::GetBuffer()
{
    return lexer.GetBuffer();
}

Node* AstBuilder::GetTree()
{
    return root;
}

std::string AstBuilder::GetTokensAsString()
{
    return lexer.GetTokensAsString();
}

std::string AstBuilder::GetTreeAsString()
{
    buffer.clear();

    TraverseAndGenerateBuffer(GetTree(), 0);

    return buffer + "\n";
}

void AstBuilder::TraverseAndGenerateBuffer(Node *node, size_t depth)
{
    for (size_t i = 0; i < depth; ++i)
        buffer += "    ";

    buffer += node->ToString() + "\n";

    for (Node* child: node->children)
        TraverseAndGenerateBuffer(child, depth + 1);
}

void AstBuilder::TraverseAndDestroyNode(Node *node)
{
    for (Node* child: node->children)
        TraverseAndDestroyNode(child);

    if (node->group == NodeGroup::Link)
    {
        Node* packageNode = static_cast<ContainerNode*>(node)->package;
        delete packageNode;
        packageNode = nullptr;

    }

    delete node;
    node = nullptr;
}

void AstBuilder::ParseNewline(size_t newlineSize)
{
    // --- BEGIN IDENTATIONS ---

    size_t currentDepth = 0;

    if (GetCurrentToken().type == TokenType::Space)
    {
        currentDepth = GetCurrentToken().size / 4;
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

    // -- BEGIN SOME_WEIRD_CONDITIONS ---
    if (!blockStack.empty() && blockStack.top() == NodeType::Table)
    {
        while (!headStack.empty() && headStack.top()->type != NodeType::Table)
            PopHead();

        PushHead(new Node(NodeType::TableRow, NodeGroup::Table));
        PushHead(new Node(NodeType::TableColumn, NodeGroup::Table));
    }
    // -- END SOME_WEIRD_CONDITIONS ---

    // --- POP ITEM NODE HEAD ---
    if (GetHead()->group == NodeGroup::Item)
        PopHead();

    switch (GetCurrentToken().type)
    {
        case TokenType::Text:
            if (blockStack.empty() || blockStack.top() != NodeType::Table)
                if (!ShouldPushHead(NodeType::Paragraph, newlineSize))
                    GetHead()->Add(new Node(NodeType::Newline));
            break;

        case TokenType::Tail:
            switch (GetCurrentToken().size)
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
            switch (GetCurrentToken().size)
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
            ParseBreak();
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
        {
            TernaryNode* ternaryNode = new TernaryNode(NodeType::CheckItem, NodeGroup::Item);
            switch (GetCurrentToken().size) {
                case 1:
                    ternaryNode->state = TernaryState::False;
                    break;
                case 2:
                    ternaryNode->state = TernaryState::InBetween;
                    break;
                case 3:
                    ternaryNode->state = TernaryState::True;
                    break;
                default:
                    break;
            }

            ShouldPushHead(NodeType::CheckList);
            PushHead(ternaryNode);
            Skip();
            break;
        }

        case TokenType::Box:
            Skip();
            ParseBlock(GetCurrentToken().type);
            break;

        default:
            break;
    }
}

void AstBuilder::ParseBreak()
{
    switch (GetCurrentToken().size)
    {
        case 1: GetHead()->Add(new Node(NodeType::LineBreak, NodeGroup::Break)); break;
        case 2: GetHead()->Add(new Node(NodeType::ThematicBreak, NodeGroup::Break)); break;
        case 3: GetHead()->Add(new Node(NodeType::PageBreak, NodeGroup::Break)); break;
    }
    Skip();
}

void AstBuilder::ParseLink(NodeType type)
{
    ContainerNode* container = new ContainerNode(type, NodeGroup::Link);
    ValueNode* node = new ValueNode();
    Skip();

    if (GetCurrentToken().type == TokenType::QuotedText)
    {
        node->type = NodeType::QuotedText;
        node->value = GetCurrentToken().value;
        Skip();
    }
    else if (GetCurrentToken().type == TokenType::Symbol)
    {
        node->type = NodeType::Symbol;
        node->value = GetCurrentToken().value;
        Skip();
    }
    container->package = node;

    if (GetCurrentToken().type == TokenType::LCurlyBracket)
    {
        PushHead(container);
        PushHead(new Node(NodeType::Wrapper));
        Skip();
    }
    else
        PushHead(container);
}

void AstBuilder::ParseBlock(TokenType type)
{
    switch (type)
    {
        case TokenType::KwTable:
            while (IsValid() && GetCurrentToken().type != TokenType::Line)
            {
                Skip();
            }
            blockStack.push(NodeType::Table);
            PushHead(new TableNode(NodeType::Table, NodeGroup::Table));
            break;

        case TokenType::KwCode:
        {
            CodeNode* codeNode = new CodeNode();
            GetHead()->Add(codeNode);
            Skip();
            if (GetCurrentToken().type == TokenType::QuotedText)
            {
                codeNode->lang = new ValueNode(NodeType::QuotedText, GetCurrentToken().value);
                Skip();
            }

            // Skip indentations
            if (GetCurrentToken().type == TokenType::Newline)
                Skip();
            if (GetCurrentToken().type == TokenType::Space)
                Skip();

            if (GetCurrentToken().type == TokenType::Line &&
                GetNextToken(1).type == TokenType::RawText &&
                GetNextToken(2).type == TokenType::Line)
            {
                codeNode->value = GetNextToken(1).value;
                Skip(3);
            }
            break;
        }

        default:
            break;
    }
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

Token& AstBuilder::GetCurrentToken()
{
    return lexer.GetToken(tokenIndex);
}

Token& AstBuilder::GetNextToken(size_t offset)
{
    return tokenIndex + offset < tokenSize ? lexer.GetToken(tokenIndex + offset) : emptyToken;
}

void AstBuilder::Skip(size_t offset)
{
    tokenIndex += offset;
}

}
