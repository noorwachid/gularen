#include "ASTBuilder.h"

namespace Gularen
{
    // PUBLIC DEFINITION

    RC<DocumentNode> ASTBuilder::Parse(const String& buffer)
    {
        Lexer lexer;
        _tokens = lexer.Parse(buffer);
        _tokenCursor = _tokens.begin();

        _documentNode = CreateRC<DocumentNode>();
        _nodeCursors.clear();
        _nodeCursors.push_back(_documentNode);
        
        while (IsInProgress())
        {
            switch (GetCurrent().type)
            {
            case TokenType::Newline:
            case TokenType::BODocument:
                ParseNewline();
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
            
            default:
                // Do nothing
                break;
            }

            Advance();
        }

        return _documentNode;
    }

    // PRIVATE DEFINITION

    // Main Routine Parsing
    
    void ASTBuilder::ParseNewline()
    {
        if (GetCurrent().size > 1 || GetCurrent().type == TokenType::BODocument)
        {
            // Should begin new segment
            PopCursorNode();

            PushCursorNode(CreateRC<ParagraphNode>());
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
        if (_nodeCursors.size() > 1)
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
