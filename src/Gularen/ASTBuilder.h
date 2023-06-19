#pragma once

#include "Lexer.h"
#include "Node.h"
#include <iostream>

namespace Gularen {
	class ASTBuilder {
	public:
		RC<RootNode> build(const Array<Token>& tokens);

		RC<RootNode> build(const String& buffer);

	private:
		// Main Routine Parsing

		void parseNewline();

		void parseIndentation(UintSize indentationLevel);

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
		UintSize indentationLevel = 0;

		Array<Token> tokens;
		Array<Token>::iterator tokenCursor;

		RC<RootNode> rootNode;
		Array<RC<Node>> nodeCursors;
	};
}
