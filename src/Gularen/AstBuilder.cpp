#include "ASTBuilder.h"
#include "Utilities/NodeWriter.h"

// #define GULAREN_DEBUG_BUFFER 1
// #define GULAREN_DEBUG_TOKENS 1
// #define GULAREN_DEBUG_AST 1

namespace Gularen
{
    // PUBLIC DEFINITION

    RC<RootNode> ASTBuilder::Parse(const String& buffer)
    {
        Lexer lexer;
        _tokens = lexer.Parse(buffer);
        _tokenCursor = _tokens.begin();

        #ifdef GULAREN_DEBUG_BUFFER
        std::cout << "[Gularen.Debug.Buffer]\n";
        std::cout << buffer << "\n\n";
        #endif

        #ifdef GULAREN_DEBUG_TOKENS
        std::cout << "[Gularen.Debug.Tokens]\n";
        for (auto a: _tokens)
            std::cout << a.ToString() << "\n";
        std::cout << "\n";
        #endif

        _rootNode = CreateRC<RootNode>();
        _nodeCursors.clear();
        _nodeCursors.push_back(_rootNode);
        
        while (IsInProgress())
        {
            switch (GetCurrent().type)
            {
            case TokenType::Newline:
            case TokenType::BODocument:
                ParseNewline();
                break;

            case TokenType::ArrowHead:
                Advance();

                if (GetCurrent().type == TokenType::Symbol && GetCursorNode()->type == NodeType::Title)
                {
                    static_cast<HeadingNode*>(_nodeCursors[_nodeCursors.size() - 2].get())->id = GetCurrent().content;
                    break;
                }

                Retreat();
                break;

            case TokenType::Text:
                AddChildCursorNode(CreateRC<TextNode>(GetCurrent().content));
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

            case TokenType::LargeArrowTail:
                // PopCursorNode();
                break;
            
            default:
                // Do nothing
                break;
            }

            Advance();
        }

        #ifdef GULAREN_DEBUG_AST
        std::cout << "[Gularen.Debug.Tokens]\n";
        NodeWriter writer;
        writer.Write(_rootNode);
        std::cout << "\n";
        #endif

        return _rootNode;
    }

    // PRIVATE DEFINITION

    // Main Routine Parsing
    
    void ASTBuilder::ParseNewline()
    {
        auto newlineSize = GetCurrent().size;

        Advance();

        auto previousType = GetCursorNode()->type;
        bool spawningNewParagraph = false;

        // Closing
        if (previousType == NodeType::Title)
        {
            PopCursorNode();
            
            // Only parse as subtitle if the segment identifier right below heading
            if (GetCurrent().type == TokenType::ArrowHead && newlineSize == 1)
                return PushCursorNode(CreateRC<SubtitleNode>());
            
            PopCursorNode();
        }
        else if (previousType == NodeType::Paragraph) 
        {
            if (GetCurrent().type != TokenType::Text || newlineSize > 1)
            {
                PopCursorNode();
            }
        }

        switch (GetCurrent().type)
        {
        case TokenType::LargeArrowTail:
            PushCursorNode(CreateRC<DocumentNode>());
            PushCursorNode(CreateRC<TitleNode>());
            break;

        case TokenType::ArrowTail:
            PushCursorNode(CreateRC<PartNode>());
            PushCursorNode(CreateRC<TitleNode>());
            break;

        case TokenType::ExtraLargeArrow:
            PushCursorNode(CreateRC<ChapterNode>());
            PushCursorNode(CreateRC<TitleNode>());
            break;

        case TokenType::LargeArrow:
            PushCursorNode(CreateRC<SectionNode>());
            PushCursorNode(CreateRC<TitleNode>());
            break;

        case TokenType::Arrow:
            PushCursorNode(CreateRC<SubsectionNode>());
            PushCursorNode(CreateRC<TitleNode>());
            break;

        case TokenType::SmallArrow:
            PushCursorNode(CreateRC<SubsubsectionNode>());
            PushCursorNode(CreateRC<TitleNode>());
            break;

        case TokenType::ArrowHead:
            PushCursorNode(CreateRC<SegmentNode>());
            PushCursorNode(CreateRC<TitleNode>());
            break;

        case TokenType::Text:
        case TokenType::Asterisk:
        case TokenType::Underscore:
        case TokenType::Backtick:
            if (previousType != NodeType::Paragraph || (previousType == NodeType::Paragraph && newlineSize > 1))
            {
                PushCursorNode(CreateRC<ParagraphNode>());
            }

            Retreat();
            break;
        
        default:
            break;
        }
    }

    void ASTBuilder::ParseFS(const RC<FSNode>& node)
    {
        if (GetCursorNode()->type == node->type)
            return PopCursorNode();

        PushCursorNode(node);
    }

    // Cursor Node Manipulation

    void ASTBuilder::PopCursorNode()
    {
        if (!_nodeCursors.empty())
            _nodeCursors.pop_back();
    }
    
    void ASTBuilder::PushCursorNode(const RC<Node>& node)
    {
        AddChildCursorNode(node);
        _nodeCursors.push_back(node);
    }

    void ASTBuilder::AddChildCursorNode(const RC<Node>& node)
    {
        _nodeCursors.back()->children.push_back(node);
    }

    const RC<Node>& ASTBuilder::GetCursorNode() const
    {
        return _nodeCursors.back();
    }

    // Token Iterator

    const Token& ASTBuilder::GetCurrent() const
    {
        return *_tokenCursor;
    }
    
    bool ASTBuilder::IsInProgress()
    {
        return _tokenCursor < _tokens.end();
    }
    
    void ASTBuilder::Advance()
    {
        ++_tokenCursor;
    }
    
    void ASTBuilder::Retreat()
    {
        --_tokenCursor;
    }
}
