#include <Gularen/Parser.h>
#include <iostream>

namespace Gularen {
	bool hasNetProtocol(const std::string& reference) {
		if (reference.empty()) return false;

		if (reference[0] >= 'a' && reference[0] <= 'z') {
			size_t i = 0;
			size_t size = 0;
			while (i < reference.size() && reference[i] >= 'a' && reference[i] <= 'z') {
				++i;
				++size;
			}

			if (i + 2 < reference.size() && 
				reference[i] == ':' && 
				reference[i + 1] == '/' &&
				reference[i + 2] == '/') {
				return true;
			}

			return false;
		}

		return false;
	}

	bool Parser::check(size_t offset) const {
		return index + offset < lexer.get().size();
	}

	bool Parser::is(size_t offset, TokenType type) const {
		return lexer.get()[index + offset].type == type;
	}

	const Token& Parser::get(size_t offset) const {
		return lexer.get()[index + offset];
	}

	void Parser::advance(size_t offset) {
		index += 1 + offset;
	}

	void Parser::add(const NodePtr& node) {
		scopes.top()->children.push_back(node);
	}

	void Parser::addScope(const NodePtr& node) {
		add(node);
		scopes.push(node);
	}

	void Parser::removeScope() {
		if (scopes.size() == 1) return;
		scopes.pop();
	}

	const NodePtr& Parser::getScope() {
		return scopes.top();
	}

	const NodePtr& Parser::parse(const std::string& content) {
		lexer.parse(content);
		index = 0;
		lastNewline = 0;
		lastIndent = 0;
		
		while (!scopes.empty()) {
			scopes.pop();
		}

		root = std::make_shared<Node>();
		scopes.push(root);

		parseBlock();

		while (check(0)) {
			parse();
		}

		return root;
	}

	const NodePtr& Parser::get() const {
		return root;
	}

	void Parser::parse() {
		switch (get(0).type) {
			case TokenType::text:
				add(std::make_shared<TextNode>(get(0).value));
				advance(0);
				break;

			case TokenType::fsBold:
				if (getScope()->group == NodeGroup::fs && static_cast<FSNode*>(getScope().get())->type == FSType::bold) {
					removeScope();
					advance(0);
					break;
				}
				addScope(std::make_shared<FSNode>(FSType::bold));
				advance(0);
				break;

			case TokenType::fsItalic:
				if (getScope()->group == NodeGroup::fs && static_cast<FSNode*>(getScope().get())->type == FSType::italic) {
					removeScope();
					advance(0);
					break;
				}
				addScope(std::make_shared<FSNode>(FSType::italic));
				advance(0);
				break;

			case TokenType::fsMonospace:
				if (getScope()->group == NodeGroup::fs && static_cast<FSNode*>(getScope().get())->type == FSType::monospace) {
					removeScope();
					advance(0);
					break;
				}
				addScope(std::make_shared<FSNode>(FSType::monospace));
				advance(0);
				break;

			case TokenType::headingMarker:
				if (get(0).count == 3) {
					addScope(std::make_shared<HeadingNode>(HeadingType::chapter));
				}
				if (get(0).count == 2) {
					addScope(std::make_shared<HeadingNode>(HeadingType::section));
				}
				if (get(0).count == 1) {
					if (!getScope()->children.empty() && getScope()->children.back()->group == NodeGroup::heading && lastNewline == 1) {
						addScope(std::make_shared<HeadingNode>(HeadingType::subtitle));
					} else {
						addScope(std::make_shared<HeadingNode>(HeadingType::subsection));
					}
				}
				advance(0);
				break;

			case TokenType::headingIDMarker:
				if (check(1) && is(1, TokenType::headingID) && getScope()->group == NodeGroup::heading) {
					static_cast<HeadingNode*>(getScope().get())->id = get(1).value;
					advance(1);
					break;
				} 
				add(std::make_shared<TextNode>(">"));
				break;

			case TokenType::break_:
				if (get(0).count == 2) {
					add(std::make_shared<BreakNode>(BreakType::page));
					advance(0);
					break;
				}
				add(std::make_shared<BreakNode>(BreakType::line));
				advance(0);
				break;

			case TokenType::pipe:
				if (getScope()->group == NodeGroup::tableCell) {
					removeScope();
				}
				if (check(1) && !is(1, TokenType::newline) && getScope()->group == NodeGroup::tableRow) {
					addScope(std::make_shared<TableCellNode>());
				}
				advance(0);
				break;

			case TokenType::reference: {
				if (hasNetProtocol(get(0).value)) {
					std::shared_ptr<LinkNode> linkNode = std::make_shared<LinkNode>(get(0).value);
					add(linkNode);
					advance(0);
					break;
				}
				std::shared_ptr<LocalLinkNode> linkNode = std::make_shared<LocalLinkNode>(get(0).value);
				add(linkNode);
				advance(0);

				if (check(0) && is(0, TokenType::referenceID)) {
					linkNode->referenceID = get(0).value;
					advance(0);
				}
				break;
			}

			case TokenType::newline:
				lastNewline = get(0).count;
				advance(0);
				parseBlock();
				break;
				
			default:
				advance(0);
				break;
		}
	}

	void Parser::parseBlock() {
		switch (getScope()->group) {
			case NodeGroup::listItem:
				removeScope();

				if (lastNewline > 1 || 
					is(0, TokenType::text) ||
					is(0, TokenType::pipe) ||
					is(0, TokenType::headingMarker)
					) {
					removeScope();
				}
				break;

			case NodeGroup::heading:
				removeScope();
				break;

			case NodeGroup::paragraph:
				if (lastNewline > 1 || 
					is(0, TokenType::bullet) ||
					is(0, TokenType::index) ||
					is(0, TokenType::checkbox) ||
					is(0, TokenType::pipe) ||
					is(0, TokenType::headingMarker)
					) {
					removeScope();
				}
				break;

			case NodeGroup::tableRow:
				removeScope();
				break;
			
			default:
				break;
		}

		if (!lastIndentCall && get(0).type != TokenType::indent) {
			parseIndent(0);
		}

		if (lastIndentCall) {
			lastIndentCall = false;
		}

		switch (get(0).type) {
			case TokenType::indent:
				parseIndent(get(0).count);
				advance(0);

				if (check(0)) {
					lastIndentCall = true;
					parseBlock();
				}
				break;

			case TokenType::bullet:
				if (getScope()->group != NodeGroup::list) {
					addScope(std::make_shared<ListNode>(ListType::bullet));
				}
				addScope(std::make_shared<ListItemNode>());
				advance(0);
				break;

			case TokenType::index:
				if (getScope()->group != NodeGroup::list) {
					addScope(std::make_shared<ListNode>(ListType::index));
				}
				addScope(std::make_shared<ListItemNode>(getScope()->children.size() + 1));
				advance(0);
				break;

			case TokenType::checkbox:
				if (getScope()->group != NodeGroup::list) {
					addScope(std::make_shared<ListNode>(ListType::index));
				}
				addScope(std::make_shared<ListItemNode>(get(0).count));
				advance(0);
				break;

			case TokenType::pipe:
				if (getScope()->group != NodeGroup::table) {
					addScope(std::make_shared<TableNode>());
				}

				if (check(1) && is(1, TokenType::pipeConnector)) {
					TableNode* tableNode = static_cast<TableNode*>(getScope().get());
					if (tableNode->header == 0) {
						tableNode->header = tableNode->children.size();
					} else {
						tableNode->footer = tableNode->children.size();
					}

					while (check(0) && (is(0, TokenType::pipe) || is(0, TokenType::pipeConnector))) {
						if (is(0, TokenType::pipeConnector)) {
							tableNode->alignments.push_back(static_cast<Alignment>(get(0).count));
						}
						advance(0);
					}
					break;
				}

				if (getScope()->group == NodeGroup::table) {
					addScope(std::make_shared<TableRowNode>());
				}
				break;

			case TokenType::text:
				if (getScope()->group != NodeGroup::paragraph) {
					addScope(std::make_shared<ParagraphNode>());
				}
				break;

			default:
				break;
		}
	}

	void Parser::parseIndent(size_t indent) {
		if (lastIndent > indent) {
			while (scopes.size() > 1 && lastIndent + 1 > indent) {
				removeScope();
				if (getScope()->group == NodeGroup::indent) {
					--lastIndent;
				}
			}
			lastIndent = indent;
		} else if (lastIndent < indent) {
			if (getScope()->group == NodeGroup::list && lastIndent + 1 == indent) {
				addScope(getScope()->children.back());
			}

			while (lastIndent < indent) {
				++lastIndent;
				addScope(std::make_shared<IndentNode>());
			}
		}
	}
}
