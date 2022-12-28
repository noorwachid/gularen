#include "ASTBuilder.h"

namespace Gularen
{
    // PUBLIC DEFINITION

    void ASTBuilder::SetBuffer(const String& buffer)
    {
        _lexer.SetBuffer(buffer);
        _lexer.Parse();
        _tokens = _lexer.GetTokens();
        _tokenCursor = _tokens.begin();
    }
    
    void ASTBuilder::SetTokens(const Array<Token>& tokens)
    {
        _tokens = tokens;
        _tokenCursor = _tokens.begin();
    }

    void ASTBuilder::Parse()
    {
        RC<Node> documentNode = CreateRC<DocumentNode>();
        _nodeCursors.clear();
        _nodeCursors.push_back(documentNode);
		_ast.SetRootNode(documentNode);
        
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
    }

	const AST& ASTBuilder::GetAST() const
	{
		return _ast;
	}

    void ASTBuilder::PrintDebugInformation()
    {
        std::cout << "[Buffer]\n";
        std::cout << _lexer.GetBuffer() << "\n";
        std::cout << "\n";

        std::cout << "[Tokens]\n";
        _lexer.PrintDebugInformation();
        std::cout << "\n";

        std::cout << "[AST]\n";            
        _ast.Print();
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
