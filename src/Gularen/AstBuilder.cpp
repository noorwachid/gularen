#include "ASTBuilder.h"
#include "Node/NodeWriter.h"
#include <iostream>

// #define GULAREN_DEBUG_BUFFER 1
// #define GULAREN_DEBUG_TOKENS 1
// #define GULAREN_DEBUG_AST 1

namespace Gularen {
	// PUBLIC DEFINITION
	RC<RootNode> ASTBuilder::build(const Array<Token>& newTokens) {
		tokens = newTokens;
		tokenCursor = tokens.begin();

#ifdef GULAREN_DEBUG_BUFFER
		std::cout << "[Gularen.Debug.Buffer]\n";
		std::cout << buffer << "\n\n";
#endif

#ifdef GULAREN_DEBUG_TOKENS
		std::cout << "[Gularen.Debug.Tokens]\n";
		for (auto a : tokens)
			std::cout << a.toString() << "\n";
		std::cout << "\n";
#endif

		rootNode = createRC<RootNode>();
		nodeCursors.clear();
		nodeCursors.push_back(rootNode);

		while (isTokenCursorInProgress()) {
			switch (getCurrentToken().type) {
				case TokenType::boDocument:
					advanceTokenCursor();

					// In case some people write with excess newlines
					if (getCurrentToken().type != TokenType::newline)
						retreatTokenCursor();

					parseNewline();
					break;

				case TokenType::newline:
					parseNewline();
					break;

				case TokenType::arrowHead:
					advanceTokenCursor();

					if (getCurrentToken().type == TokenType::symbol && getNodeCursor()->type == NodeType::title) {
						static_cast<HeadingNode*>(nodeCursors[nodeCursors.size() - 2].get())->id =
							getCurrentToken().content;
						break;
					}

					retreatTokenCursor();
					break;

				case TokenType::text:
					addNodeCursorChild(createRC<TextNode>(getCurrentToken().content));
					break;

				case TokenType::rArrowHead:
					addNodeCursorChild(createRC<LineBreakNode>());
					break;

				case TokenType::asterisk:
					parseFS(createRC<BoldFSNode>());
					break;

				case TokenType::underscore:
					parseFS(createRC<ItalicFSNode>());
					break;

				case TokenType::Backtick:
					parseFS(createRC<MonospaceFSNode>());
					break;

				case TokenType::lArrowTail:
					// popNodeCursor();
					break;

				default:
					// Do nothing
					break;
			}

			advanceTokenCursor();
		}

#ifdef GULAREN_DEBUG_AST
		std::cout << "[Gularen.Debug.Tokens]\n";
		NodeWriter writer;
		writer.write(rootNode);
		std::cout << "\n";
#endif

		return rootNode;
	}

	RC<RootNode> ASTBuilder::build(const String& buffer) {
		return build(Lexer().tokenize(buffer));
	}

	// PRIVATE DEFINITION

	// Main Routine Parsing

	void ASTBuilder::parseNewline() {
		auto newlineSize = getCurrentToken().size;

		advanceTokenCursor();

		// Indentation is independent from any other parsing type
		if (getCurrentToken().type == TokenType::indentation) {
			UintSize indentationLevel = getCurrentToken().size;
			advanceTokenCursor();
			parseIndentation(indentationLevel);
		} else {
			parseIndentation(0);
		}

		bool spawningNewParagraph = false;

		// Closing
		if (getNodeCursor()->type != NodeType::indentation) {
			if (getNodeCursor()->type == NodeType::title) {
				popNodeCursor();

				// Only parse as subtitle if the segment identifier right below heading
				if (getCurrentToken().type == TokenType::arrowHead && newlineSize == 1)
					return pushNodeCursor(createRC<SubtitleNode>());

				popNodeCursor();
			} else if (getNodeCursor()->type == NodeType::paragraph) {
				if (getCurrentToken().type != TokenType::text || newlineSize > 1) {
					popNodeCursor();
				}
			} else {
				popNodeCursor();
			}
		}

		switch (getCurrentToken().type) {
			case TokenType::lArrowTail:
				pushNodeCursor(createRC<DocumentNode>());
				pushNodeCursor(createRC<TitleNode>());
				break;

			case TokenType::arrowTail:
				pushNodeCursor(createRC<PartNode>());
				pushNodeCursor(createRC<TitleNode>());
				break;

			case TokenType::xlArrow:
				pushNodeCursor(createRC<ChapterNode>());
				pushNodeCursor(createRC<TitleNode>());
				break;

			case TokenType::lArrow:
				pushNodeCursor(createRC<SectionNode>());
				pushNodeCursor(createRC<TitleNode>());
				break;

			case TokenType::arrow:
				pushNodeCursor(createRC<SubsectionNode>());
				pushNodeCursor(createRC<TitleNode>());
				break;

			case TokenType::sArrow:
				pushNodeCursor(createRC<SubsubsectionNode>());
				pushNodeCursor(createRC<TitleNode>());
				break;

			case TokenType::arrowHead:
				pushNodeCursor(createRC<SegmentNode>());
				pushNodeCursor(createRC<TitleNode>());
				break;

			case TokenType::rArrowTail:
				addNodeCursorChild(createRC<PageBreakNode>());
				break;

			case TokenType::rlArrowTail:
				advanceTokenCursor();
				tokenCursor = tokens.end();
				break;

			case TokenType::numericBullet:
				if (getNodeCursor()->type != NodeType::numericList)
					pushNodeCursor(createRC<NumericListNode>());

				pushNodeCursor(createRC<NumericItemNode>());
				break;

			case TokenType::text:
			case TokenType::asterisk:
			case TokenType::underscore:
			case TokenType::Backtick:
				if (getNodeCursor()->type != NodeType::paragraph ||
					(getNodeCursor()->type == NodeType::paragraph && newlineSize > 1)) {
					pushNodeCursor(createRC<ParagraphNode>());
				} else {
					addNodeCursorChild(createRC<NewlineNode>(1));
				}

				retreatTokenCursor();
				break;

			default:
				break;
		}
	}

	void ASTBuilder::parseIndentation(UintSize currentIndentationLevel) {
		if (indentationLevel == currentIndentationLevel)
			return;

		if (indentationLevel < currentIndentationLevel) {
			UintSize difference = currentIndentationLevel - indentationLevel;

			while (nodeCursors.size() > 1 && getNodeCursor()->group != NodeGroup::Block) {
				popNodeCursor();
			}

			popNodeCursor();

			for (UintSize i = 0; i < difference; ++i) {
				pushNodeCursor(createRC<IndentationNode>());
			}

			indentationLevel = currentIndentationLevel;
			return;
		}

		UintSize difference = indentationLevel - currentIndentationLevel;

		for (UintSize i = 0; i < difference && nodeCursors.size() > 1; ++i) {
			while (nodeCursors.size() > 1 && getNodeCursor()->type != NodeType::indentation) {
				popNodeCursor();
			}
			popNodeCursor();
		}

		indentationLevel = currentIndentationLevel;
		return;
	}

	void ASTBuilder::parseFS(const RC<FSNode>& node) {
		if (getNodeCursor()->type == node->type)
			return popNodeCursor();

		pushNodeCursor(node);
	}

	// Cursor Node Manipulation

	void ASTBuilder::popNodeCursor() {
		if (nodeCursors.size() > 1)
			nodeCursors.pop_back();
	}

	void ASTBuilder::pushNodeCursor(const RC<Node>& node) {
		addNodeCursorChild(node);
		nodeCursors.push_back(node);
	}

	void ASTBuilder::addNodeCursorChild(const RC<Node>& node) {
		nodeCursors.back()->children.push_back(node);
	}

	const RC<Node>& ASTBuilder::getNodeCursor() const {
		return nodeCursors.back();
	}

	// Token Iterator

	const Token& ASTBuilder::getCurrentToken() const {
		return *tokenCursor;
	}

	bool ASTBuilder::isTokenCursorInProgress() {
		return tokenCursor < tokens.end();
	}

	void ASTBuilder::advanceTokenCursor() {
		++tokenCursor;
	}

	void ASTBuilder::retreatTokenCursor() {
		--tokenCursor;
	}
}
