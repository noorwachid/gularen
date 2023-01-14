#pragma once

#include "Lexer.h"
#include "Node.h"
#include <iostream>

namespace Gularen
{
    class ASTBuilder
    {
      public:
        RC<RootNode> Build(const Array<Token>& tokens);

        RC<RootNode> Build(const String& buffer);

      private:
        // Main Routine Parsing

        void ParseNewline();

        void ParseIndentation(UintSize indentationLevel);

        void ParseFS(const RC<FSNode>& node);

        // Cursor Node Manipulation

        void PopNodeCursor();

        void PushNodeCursor(const RC<Node>& node);

        void AddNodeCursorChild(const RC<Node>& node);

        const RC<Node>& GetNodeCursor() const;

        // Token Iterator

        const Token& GetCurrentToken() const;

        bool IsTokenCursorInProgress();

        void AdvanceTokenCursor();

        void RetreatTokenCursor();

      private:
        UintSize _indentationLevel = 0;

        Array<Token> _tokens;
        Array<Token>::iterator _tokenCursor;

        RC<RootNode> _rootNode;
        Array<RC<Node>> _nodeCursors;
    };
}
