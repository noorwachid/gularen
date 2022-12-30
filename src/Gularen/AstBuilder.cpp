#include "ASTBuilder.h"
#include "Utility/NodeWriter.h"

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
            case TokenType::Text:
                AddChildCursorNode(CreateRC<TextNode>(GetCurrent().content));
                break;
                
            case TokenType::LSQuote:
                AddChildCursorNode(CreateRC<QuoteNode>(NodeType::LSQuote));
                break;

            case TokenType::RSQuote:
                AddChildCursorNode(CreateRC<QuoteNode>(NodeType::RSQuote));
                break;

            case TokenType::LDQuote:
                AddChildCursorNode(CreateRC<QuoteNode>(NodeType::LDQuote));
                break;

            case TokenType::RDQuote:
                AddChildCursorNode(CreateRC<QuoteNode>(NodeType::RDQuote));
                break;
                
            case TokenType::Asterisk:
                ParseFS(NodeType::BoldFS);
                break;

            case TokenType::Underscore:
                ParseFS(NodeType::ItalicFS);
                break;

            case TokenType::Backtick:
                ParseFS(NodeType::MonospacedFS);
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

    void ASTBuilder::ParseFS(NodeType type)
    {
        if (GetCursorNode()->type == type)
            return PopCursorNode();

        PushCursorNode(CreateRC<FSNode>(type));
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
