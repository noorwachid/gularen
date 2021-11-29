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
                GetHead()->Add(new ValueNode(NodeType::Text, NodeGroup::Text, NodeShape::InBetween, GetCurrentToken().value));
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
                    GetHead()->Add(new ValueNode(NodeType::InlineCode, NodeGroup::Text, NodeShape::InBetween, GetCurrentToken().value));
                    Skip(2);
                }
                break;
            }

            case TokenType::LCurlyBracket:
                Skip();
                switch (GetCurrentToken().type)
                {
                    case TokenType::Symbol:
                        GetHead()->Add(new ValueNode(NodeType::Curtain, NodeGroup::Text, NodeShape::InBetween, GetCurrentToken().value));
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
                while (heads.size() > 1 && heads.top()->type != NodeType::TableRow)
                    PopHead();
                PushHead(new Node(NodeType::TableColumn, NodeGroup::Table));
                Skip();
                break;

            case TokenType::HashSymbol:
                GetHead()->Add(new ValueNode(NodeType::HashSymbol, NodeGroup::Tag, NodeShape::InBetween, GetCurrentToken().value));
                Skip();
                break;
            case TokenType::AtSymbol:
                GetHead()->Add(new ValueNode(NodeType::AtSymbol, NodeGroup::Tag, NodeShape::InBetween, GetCurrentToken().value));
                Skip();
                break;

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

    while (!heads.empty())
        heads.pop();

    heads.push(root);
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
    ParseIndentation();

    switch (GetCurrentToken().type)
    {
        case TokenType::Tail:
            switch (GetCurrentToken().size)
            {
                case 1:
                    CompareAndPopHead(NodeType::Minisection, newlineSize);
                    PushHead(new ValueNode(NodeType::Minisection, NodeGroup::Header, NodeShape::Line));
                    break;

                case 2:
                    CompareAndPopHead(NodeType::Part, newlineSize);
                    PushHead(new ValueNode(NodeType::Part, NodeGroup::Header, NodeShape::Line));
                    break;

                case 3:
                    CompareAndPopHead(NodeType::Title, newlineSize);
                    PushHead(new ValueNode(NodeType::Title, NodeGroup::Header, NodeShape::Line));
                    break;

                default:
                    break;
            }
            Skip();
            break;

        case TokenType::RevTail:
            if (GetCurrentToken().size == 2)
            {
                CompareAndPopHead(NodeType::ThematicBreak);
                GetHead()->Add(new Node(NodeType::ThematicBreak, NodeGroup::Break, NodeShape::Line));
                Skip();
            }
            else if (GetCurrentToken().size > 2)
            {
                CompareAndPopHead(NodeType::PageBreak);
                GetHead()->Add(new Node(NodeType::PageBreak, NodeGroup::Break, NodeShape::Line));
                Skip();
            }
            break;

        case TokenType::Arrow:
            switch (GetCurrentToken().size)
            {
                case 1:
                    CompareAndPopHead(NodeType::Subsubsection, newlineSize);
                    PushHead(new ValueNode(NodeType::Subsubsection, NodeGroup::Header, NodeShape::Line));
                    break;

                case 2:
                    CompareAndPopHead(NodeType::Subsection, newlineSize);
                    PushHead(new ValueNode(NodeType::Subsection, NodeGroup::Header, NodeShape::Line));
                    break;

                case 3:
                    CompareAndPopHead(NodeType::Section, newlineSize);
                    PushHead(new ValueNode(NodeType::Section, NodeGroup::Header, NodeShape::Line));
                    break;

                case 4:
                    CompareAndPopHead(NodeType::Chapter, newlineSize);
                    PushHead(new ValueNode(NodeType::Chapter, NodeGroup::Header, NodeShape::Line));
                    break;

                default:
                    break;
            }

            break;

        case TokenType::Bullet:
            CompareAndPopHead(NodeType::List);

            if (GetHead()->type != NodeType::List)
                PushHead(new Node(NodeType::List, NodeGroup::List, NodeShape::Block));

            PushHead(new Node(NodeType::Item, NodeGroup::Item, NodeShape::Block));
            Skip();
            break;

        case TokenType::NBullet:
            CompareAndPopHead(NodeType::NList);

            if (GetHead()->type != NodeType::NList)
                PushHead(new Node(NodeType::NList, NodeGroup::List, NodeShape::Block));

            PushHead(new Node(NodeType::Item, NodeGroup::Item, NodeShape::Block));
            Skip();
            break;

        case TokenType::CheckBox:
            CompareAndPopHead(NodeType::CheckList);

            if (GetHead()->type != NodeType::CheckList)
                PushHead(new Node(NodeType::CheckList, NodeGroup::List, NodeShape::Block));

            PushHead(new TernaryNode(NodeType::CheckItem, NodeGroup::Item, NodeShape::Block, static_cast<TernaryState>(GetCurrentToken().size)));
            Skip();
            break;

        case TokenType::Dollar:
            Skip();
            ParseBlock(GetCurrentToken().type);
            break;

        case TokenType::Line:
            while (heads.size() > 1 && heads.top()->shape != NodeShape::Block)
                heads.pop();

            PopHead();
            Skip();
            break;

        default:
            if (GetHead()->group != NodeGroup::Table)
            {
                CompareAndPopHead(NodeType::Paragraph, newlineSize);

                if (GetHead()->type != NodeType::Paragraph)
                    PushHead(new Node(NodeType::Paragraph));
                else
                    GetHead()->Add(new Node(NodeType::Newline));
            }
            else
            {
                CompareAndPopHead(NodeType::Table);
                PushHead(new Node(NodeType::TableRow, NodeGroup::Table, NodeShape::Line));
                PushHead(new Node(NodeType::TableColumn, NodeGroup::Table, NodeShape::Line));
            }
            break;
    }
}

void AstBuilder::ParseIndentation()
{
    size_t currentDepth = 0;

    if (GetCurrentToken().type == TokenType::Space)
    {
        currentDepth = GetCurrentToken().size / 4;
        Skip();
    }

    if (currentDepth > depth)
    {
        size_t distance = currentDepth - depth - 1;
        for (size_t i = 0; i < distance; ++i)
            PushHead(new Node(NodeType::Indent, NodeGroup::Wrapper, NodeShape::SuperBlock));

        PushHead(new Node(
            GetHead()->group != NodeGroup::Item ? NodeType::Indent : NodeType::Wrapper,
            NodeGroup::Wrapper,
            NodeShape::SuperBlock));
    }
    else if (currentDepth < depth)
    {
        size_t distance = depth - currentDepth;
        size_t i = 0;
        while (i < distance)
        {
            if (GetHead()->type == NodeType::Root)
                break;

            if (GetHead()->shape == NodeShape::SuperBlock)
                ++i;

            PopHead();
        }
    }

    depth = currentDepth;
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
            CompareAndPopHead(NodeType::Table);

            while (IsValid() && GetCurrentToken().type != TokenType::Line)
            {
                Skip();
            }
            PushHead(new TableNode());
            break;

        case TokenType::KwCode:
        {
            CompareAndPopHead(NodeType::Code);

            CodeNode* codeNode = new CodeNode();
            GetHead()->Add(codeNode);
            Skip();
            if (GetCurrentToken().type == TokenType::QuotedText)
            {
                codeNode->lang = new ValueNode(NodeType::QuotedText, NodeGroup::Text, NodeShape::Block, GetCurrentToken().value);
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

        case TokenType::KwToc:
            while (IsValid() && GetCurrentToken().type != TokenType::Newline)
                Skip();
            GetHead()->Add(new Node(NodeType::Toc, NodeGroup::Unknown, NodeShape::Line));
            break;

        case TokenType::KwFile:
            while (IsValid() && GetCurrentToken().type != TokenType::Newline)
                Skip();
            GetHead()->Add(new Node(NodeType::File, NodeGroup::Unknown, NodeShape::Line));
            break;

        case TokenType::KwImage:
            while (IsValid() && GetCurrentToken().type != TokenType::Newline)
                Skip();
            GetHead()->Add(new Node(NodeType::Image, NodeGroup::Unknown, NodeShape::Line));
            break;

        case TokenType::KwAdmon:
            while (IsValid() && GetCurrentToken().type != TokenType::Line)
                Skip();
            GetHead()->Add(new Node(NodeType::Admon, NodeGroup::Unknown, NodeShape::Block));
            break;

//        case TokenType::Symbol:
//            while (IsValid() && GetCurrentToken().type != TokenType::Line)
//                Skip();
//            GetHead()->Add(new Node(NodeType::Assignment, NodeGroup::Unknown, NodeShape::Line));
//            break;

        default:
            break;
    }
}

Node* AstBuilder::GetHead()
{
    return heads.top();
}

void AstBuilder::PushHead(Node* node)
{
    GetHead()->Add(node);
    heads.push(node);
}

void AstBuilder::CompareAndPopHead(NodeType type)
{
    if (heads.size() > 1)
    {
        if (heads.top()->type != type)
            while (heads.size() > 1 && heads.top()->type != type && heads.top()->shape != NodeShape::SuperBlock)
                heads.pop();
        else if (heads.top()->shape == NodeShape::Line)
            heads.pop();
    }
}

void AstBuilder::CompareAndPopHead(NodeType type, size_t newlineSize)
{
    if (heads.size() > 1)
    {
        if (heads.top()->type != type)
            while (heads.size() > 1 && heads.top()->type != type && heads.top()->shape != NodeShape::SuperBlock)
                heads.pop();
        else if (newlineSize > 1 || heads.top()->shape == NodeShape::Line)
            heads.pop();
    }

}

void AstBuilder::PopHead()
{
    if (heads.size() > 1)
        heads.pop();
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
