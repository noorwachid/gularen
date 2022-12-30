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
                ParseClosingNewline();
                ParseOpeningNewline();
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

    void ASTBuilder::ParseOpeningNewline()
    {
        PushCursorNode(CreateRC<ParagraphNode>());
    }

    void ASTBuilder::ParseClosingNewline()
    {
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
