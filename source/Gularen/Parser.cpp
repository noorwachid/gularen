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
		std::cout << std::string((scopes.size() - 1) * 2, ' ') << node->toString() << '\n';
	}

	void Parser::addScope(const NodePtr& node) {
		add(node);
		scopes.push(node);
	}

	void Parser::removeScope() {
		if (scopes.empty()) return;
		scopes.pop();
	}

	const NodePtr& Parser::getScope() const {
		return scopes.top();
	}

	const NodePtr& Parser::parse(const std::string& content) {
		lexer.parse(content);
		index = 0;
		
		while (!scopes.empty()) {
			scopes.pop();
		}

		root = std::make_shared<Node>();
		scopes.push(root);

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
				if (getScope()->type == NodeType::fsBold) {
					removeScope();
					advance(0);
					break;
				}
				addScope(std::make_shared<Node>(NodeType::fsBold));
				advance(0);
				break;

			case TokenType::fsItalic:
				if (getScope()->type == NodeType::fsItalic) {
					removeScope();
					advance(0);
					break;
				}
				addScope(std::make_shared<Node>(NodeType::fsItalic));
				advance(0);
				break;

			case TokenType::fsMonospace:
				if (getScope()->type == NodeType::fsMonospace) {
					removeScope();
					advance(0);
					break;
				}
				addScope(std::make_shared<Node>(NodeType::fsMonospace));
				advance(0);
				break;
				
			default:
				advance(0);
				break;
		}
	}
}
