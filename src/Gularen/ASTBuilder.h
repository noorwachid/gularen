#pragma once

#include "Lexer.h"
#include "Node.h"
#include <iostream>

namespace Gularen
{
    class ASTBuilder
    {
    public:
        RC<DocumentNode> Parse(const String& buffer);
    
    private:
        // Main Routine Parsing
        
        void ParseNewline();

        void ParseFS(const RC<FSNode>& node);

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
        Array<Token> _tokens;
        Array<Token>::iterator _tokenCursor;
        
        RC<DocumentNode> _documentNode;
        Array<RC<Node>> _nodeCursors;
    };
}
