#include <Gularen/Parser.h>
#include <iostream>

namespace Gularen {
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
			case NodeGroup::heading:
				removeScope();
				break;

			case NodeGroup::paragraph:
				if (lastNewline > 1) {
					removeScope();
				}
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

		std::cout << "lastIndent: " << lastIndent << '\n';
		std::cout << "token: " << get(0).value << '\n';

		switch (get(0).type) {
			case TokenType::indent:
				parseIndent(get(0).count);
				advance(0);

				if (check(0)) {
					lastIndentCall = true;
					parseBlock();
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
			std::cout << "down lastIndent: " << lastIndent << '\n';
		} else if (lastIndent < indent) {
			while (lastIndent < indent) {
				++lastIndent;
				addScope(std::make_shared<IndentNode>());
			}
			std::cout << "up lastIndent: " << lastIndent << '\n';
		}
	}
}
