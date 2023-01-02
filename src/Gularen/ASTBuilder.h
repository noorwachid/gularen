#pragma once

#include "Lexer.h"
#include "Node.h"
#include <iostream>

namespace Gularen {
    class ASTBuilder {
    public:
        RC<RootNode> parse(const String& buffer);
    
    private:
        // Main Routine Parsing
        
        void parseNewline();

        void parseFS(const RC<FSNode>& node);

        // Cursor Node Manipulation

        void popNodeCursor();

        void pushNodeCursor(const RC<Node>& node);

        void addNodeCursorChild(const RC<Node>& node);

        const RC<Node>& getNodeCursor() const;

        // Token Iterator

        const Token& getCurrentToken() const;
        
        bool isTokenCursorInProgress();
        
        void advanceTokenCursor();
        
        void retreatTokenCursor();

    private:
        Array<Token> _tokens;
        Array<Token>::iterator _tokenCursor;
        
        RC<RootNode> _rootNode;
        Array<RC<Node>> _nodeCursors;
    };
}
