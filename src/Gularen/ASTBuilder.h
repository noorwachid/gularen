#pragma once

#include "Lexer.h"
#include "AST.h"
#include <iostream>

namespace Gularen
{
    class ASTBuilder
    {
    public:
        void SetBuffer(const String& buffer);

        void SetTokens(const Array<Token>& tokens);
        
        void Parse();
    
        void PrintDebugInformation();

		const AST& GetAST() const;

    private:
        // Main Routine Parsing

        void ParseFS(NodeType type);

        // Cursor Node Manipulation

        void PopCursorNode();

        void PushCursorNode(const RC<Node>& node);

        void AddChildCursorNode(const RC<Node>& node);

        const RC<Node>& GetCursorNode() const;

        // Token Iterator

        const Token& GetCurrent() const;
        
        bool IsInProgress();
        
        void Advance();
        
        void Retreat();

    private:
        Lexer _lexer;

        Array<Token> _tokens;
        Array<Token>::iterator _tokenCursor;
        
        AST _ast;
        Array<RC<Node>> _nodeCursors;
    };
}
