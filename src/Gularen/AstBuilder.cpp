#include "ASTBuilder.h"
#include "Utilities/NodeWriter.h"
#include <iostream>

// #define GULAREN_DEBUG_BUFFER 1
// #define GULAREN_DEBUG_TOKENS 1
// #define GULAREN_DEBUG_AST 1

namespace Gularen {
    // PUBLIC DEFINITION

    RC<RootNode> ASTBuilder::build(const String& buffer) {
        Lexer lexer;
        tokens = lexer.tokenize(buffer);
        tokenCursor = tokens.begin();

        #ifdef GULAREN_DEBUG_BUFFER
        std::cout << "[Gularen.Debug.Buffer]\n";
        std::cout << buffer << "\n\n";
        #endif

        #ifdef GULAREN_DEBUG_TOKENS
        std::cout << "[Gularen.Debug.Tokens]\n";
        for (auto a: tokens)
            std::cout << a.toString() << "\n";
        std::cout << "\n";
        #endif

        rootNode = makeRC<RootNode>();
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
                    static_cast<HeadingNode*>(nodeCursors[nodeCursors.size() - 2].get())->id = getCurrentToken().content;
                    break;
                }

                retreatTokenCursor();
                break;

            case TokenType::text:
                addNodeCursorChild(makeRC<TextNode>(getCurrentToken().content));
                break;

            case TokenType::reverseArrowHead:
                addNodeCursorChild(makeRC<LineBreakNode>());
                break;
                
            case TokenType::asterisk:
                parseFS(makeRC<BoldFSNode>());
                break;

            case TokenType::underscore:
                parseFS(makeRC<ItalicFSNode>());
                break;

            case TokenType::backtick:
                parseFS(makeRC<MonospaceFSNode>());
                break;

            case TokenType::largeArrowTail:
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

    const Array<Token>& ASTBuilder::getTokenCollection() const {
        return tokens;
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
                    return pushNodeCursor(makeRC<SubtitleNode>());
                
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
        case TokenType::largeArrowTail:
            pushNodeCursor(makeRC<DocumentNode>());
            pushNodeCursor(makeRC<TitleNode>());
            break;

        case TokenType::arrowTail:
            pushNodeCursor(makeRC<PartNode>());
            pushNodeCursor(makeRC<TitleNode>());
            break;

        case TokenType::extraLargeArrow:
            pushNodeCursor(makeRC<ChapterNode>());
            pushNodeCursor(makeRC<TitleNode>());
            break;

        case TokenType::largeArrow:
            pushNodeCursor(makeRC<SectionNode>());
            pushNodeCursor(makeRC<TitleNode>());
            break;

        case TokenType::arrow:
            pushNodeCursor(makeRC<SubsectionNode>());
            pushNodeCursor(makeRC<TitleNode>());
            break;

        case TokenType::smallArrow:
            pushNodeCursor(makeRC<SubsubsectionNode>());
            pushNodeCursor(makeRC<TitleNode>());
            break;

        case TokenType::arrowHead:
            pushNodeCursor(makeRC<SegmentNode>());
            pushNodeCursor(makeRC<TitleNode>());
            break;

        case TokenType::reverseArrowTail:
            addNodeCursorChild(makeRC<PageBreakNode>());
            break;

        case TokenType::reverseLargeArrowTail:
            advanceTokenCursor();
            // TODO: stop the parsing and send error
            tokenCursor = tokens.end();

            rootNode = makeRC<RootNode>();
            nodeCursors.clear();
            nodeCursors.push_back(rootNode);
            break;

        case TokenType::text:
        case TokenType::asterisk:
        case TokenType::underscore:
        case TokenType::backtick:
            if (getNodeCursor()->type != NodeType::paragraph || (getNodeCursor()->type == NodeType::paragraph && newlineSize > 1)) {
                pushNodeCursor(makeRC<ParagraphNode>());
            } else {
                addNodeCursorChild(makeRC<NewlineNode>(1));
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

            while (nodeCursors.size() > 1 && getNodeCursor()->group != NodeGroup::block) {
                popNodeCursor();
            }

            popNodeCursor();

            for (UintSize i = 0; i < difference; ++i) {
			    pushNodeCursor(makeRC<IndentationNode>());
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
