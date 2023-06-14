#include "ASTBuilder.h"
#include "Node/NodeWriter.h"
#include <iostream>

// #define GULAREN_DEBUG_BUFFER 1
// #define GULAREN_DEBUG_TOKENS 1
// #define GULAREN_DEBUG_AST 1

namespace Gularen
{
    // PUBLIC DEFINITION
    RC<RootNode> ASTBuilder::Build(const Array<Token>& newTokens)
    {
        tokens = newTokens;
        tokenCursor = tokens.begin();

#ifdef GULAREN_DEBUG_BUFFER
        std::cout << "[Gularen.Debug.Buffer]\n";
        std::cout << buffer << "\n\n";
#endif

#ifdef GULAREN_DEBUG_TOKENS
        std::cout << "[Gularen.Debug.Tokens]\n";
        for (auto a : tokens)
            std::cout << a.toString() << "\n";
        std::cout << "\n";
#endif

        rootNode = CreateRC<RootNode>();
        nodeCursors.clear();
        nodeCursors.push_back(rootNode);

        while (IsTokenCursorInProgress())
        {
            switch (GetCurrentToken().type)
            {
            case TokenType::BODocument:
                AdvanceTokenCursor();

                // In case some people write with excess newlines
                if (GetCurrentToken().type != TokenType::Newline)
                    RetreatTokenCursor();

                ParseNewline();
                break;

            case TokenType::Newline:
                ParseNewline();
                break;

            case TokenType::ArrowHead:
                AdvanceTokenCursor();

                if (GetCurrentToken().type == TokenType::Symbol && GetNodeCursor()->type == NodeType::Title)
                {
                    static_cast<HeadingNode*>(nodeCursors[nodeCursors.size() - 2].get())->id =
                        GetCurrentToken().content;
                    break;
                }

                RetreatTokenCursor();
                break;

            case TokenType::Text:
                AddNodeCursorChild(CreateRC<TextNode>(GetCurrentToken().content));
                break;

            case TokenType::RArrowHead:
                AddNodeCursorChild(CreateRC<LineBreakNode>());
                break;

            case TokenType::Asterisk:
                ParseFS(CreateRC<BoldFSNode>());
                break;

            case TokenType::Underscore:
                ParseFS(CreateRC<ItalicFSNode>());
                break;

            case TokenType::Backtick:
                ParseFS(CreateRC<MonospaceFSNode>());
                break;

            case TokenType::LArrowTail:
                // popNodeCursor();
                break;

            default:
                // Do nothing
                break;
            }

            AdvanceTokenCursor();
        }

#ifdef GULAREN_DEBUG_AST
        std::cout << "[Gularen.Debug.Tokens]\n";
        NodeWriter writer;
        writer.write(rootNode);
        std::cout << "\n";
#endif

        return rootNode;
    }

    RC<RootNode> ASTBuilder::Build(const String& buffer)
    {
        return Build(Lexer().Tokenize(buffer));
    }

    // PRIVATE DEFINITION

    // Main Routine Parsing

    void ASTBuilder::ParseNewline()
    {
        auto newlineSize = GetCurrentToken().size;

        AdvanceTokenCursor();

        // Indentation is independent from any other parsing type
        if (GetCurrentToken().type == TokenType::Indentation)
        {
            UintSize indentationLevel = GetCurrentToken().size;
            AdvanceTokenCursor();
            ParseIndentation(indentationLevel);
        }
        else
        {
            ParseIndentation(0);
        }

        bool spawningNewParagraph = false;

        // Closing
        if (GetNodeCursor()->type != NodeType::Indentation)
        {
            if (GetNodeCursor()->type == NodeType::Title)
            {
                PopNodeCursor();

                // Only parse as subtitle if the segment identifier right below heading
                if (GetCurrentToken().type == TokenType::ArrowHead && newlineSize == 1)
                    return PushNodeCursor(CreateRC<SubtitleNode>());

                PopNodeCursor();
            }
            else if (GetNodeCursor()->type == NodeType::Paragraph)
            {
                if (GetCurrentToken().type != TokenType::Text || newlineSize > 1)
                {
                    PopNodeCursor();
                }
            }
            else
            {
                PopNodeCursor();
            }
        }

        switch (GetCurrentToken().type)
        {
        case TokenType::LArrowTail:
            PushNodeCursor(CreateRC<DocumentNode>());
            PushNodeCursor(CreateRC<TitleNode>());
            break;

        case TokenType::ArrowTail:
            PushNodeCursor(CreateRC<PartNode>());
            PushNodeCursor(CreateRC<TitleNode>());
            break;

        case TokenType::XLArrow:
            PushNodeCursor(CreateRC<ChapterNode>());
            PushNodeCursor(CreateRC<TitleNode>());
            break;

        case TokenType::LArrow:
            PushNodeCursor(CreateRC<SectionNode>());
            PushNodeCursor(CreateRC<TitleNode>());
            break;

        case TokenType::Arrow:
            PushNodeCursor(CreateRC<SubsectionNode>());
            PushNodeCursor(CreateRC<TitleNode>());
            break;

        case TokenType::SArrow:
            PushNodeCursor(CreateRC<SubsubsectionNode>());
            PushNodeCursor(CreateRC<TitleNode>());
            break;

        case TokenType::ArrowHead:
            PushNodeCursor(CreateRC<SegmentNode>());
            PushNodeCursor(CreateRC<TitleNode>());
            break;

        case TokenType::RArrowTail:
            AddNodeCursorChild(CreateRC<PageBreakNode>());
            break;

        case TokenType::RLArrowTail:
            AdvanceTokenCursor();
            tokenCursor = tokens.end();
            break;

        case TokenType::NumericBullet:
            if (GetNodeCursor()->type != NodeType::NumericList) 
                PushNodeCursor(CreateRC<NumericListNode>());

            PushNodeCursor(CreateRC<NumericItemNode>());
            break;


        case TokenType::Text:
        case TokenType::Asterisk:
        case TokenType::Underscore:
        case TokenType::Backtick:
            if (GetNodeCursor()->type != NodeType::Paragraph ||
                (GetNodeCursor()->type == NodeType::Paragraph && newlineSize > 1))
            {
                PushNodeCursor(CreateRC<ParagraphNode>());
            }
            else
            {
                AddNodeCursorChild(CreateRC<NewlineNode>(1));
            }

            RetreatTokenCursor();
            break;

        default:
            break;
        }
    }

    void ASTBuilder::ParseIndentation(UintSize currentIndentationLevel)
    {
        if (indentationLevel == currentIndentationLevel)
            return;

        if (indentationLevel < currentIndentationLevel)
        {
            UintSize difference = currentIndentationLevel - indentationLevel;

            while (nodeCursors.size() > 1 && GetNodeCursor()->group != NodeGroup::Block)
            {
                PopNodeCursor();
            }

            PopNodeCursor();

            for (UintSize i = 0; i < difference; ++i)
            {
                PushNodeCursor(CreateRC<IndentationNode>());
            }

            indentationLevel = currentIndentationLevel;
            return;
        }

        UintSize difference = indentationLevel - currentIndentationLevel;

        for (UintSize i = 0; i < difference && nodeCursors.size() > 1; ++i)
        {
            while (nodeCursors.size() > 1 && GetNodeCursor()->type != NodeType::Indentation)
            {
                PopNodeCursor();
            }
            PopNodeCursor();
        }

        indentationLevel = currentIndentationLevel;
        return;
    }

    void ASTBuilder::ParseFS(const RC<FSNode>& node)
    {
        if (GetNodeCursor()->type == node->type)
            return PopNodeCursor();

        PushNodeCursor(node);
    }

    // Cursor Node Manipulation

    void ASTBuilder::PopNodeCursor()
    {
        if (nodeCursors.size() > 1)
            nodeCursors.pop_back();
    }

    void ASTBuilder::PushNodeCursor(const RC<Node>& node)
    {
        AddNodeCursorChild(node);
        nodeCursors.push_back(node);
    }

    void ASTBuilder::AddNodeCursorChild(const RC<Node>& node) { nodeCursors.back()->children.push_back(node); }

    const RC<Node>& ASTBuilder::GetNodeCursor() const { return nodeCursors.back(); }

    // Token Iterator

    const Token& ASTBuilder::GetCurrentToken() const { return *tokenCursor; }

    bool ASTBuilder::IsTokenCursorInProgress() { return tokenCursor < tokens.end(); }

    void ASTBuilder::AdvanceTokenCursor() { ++tokenCursor; }

    void ASTBuilder::RetreatTokenCursor() { --tokenCursor; }
}
