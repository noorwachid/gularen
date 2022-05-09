#include "AstBuilder.hpp"
#include "IO.hpp"

namespace Gularen {
	AstBuilder::AstBuilder() {
	}

	AstBuilder::~AstBuilder() {
		if (rootNode)
			destroyTree();
	}

	void AstBuilder::setBuffer(const std::string& buffer) {
		lexer.setBuffer(buffer);
		lexer.parse();

		reset();
	}

	void AstBuilder::setTokens(const std::vector<Token>& tokens) {
		lexer.setTokens(tokens);

		reset();
	}

	void AstBuilder::parse() {
		if (getCurrentToken().type == TokenType::openDocument)
			skip();

		parseNewline();

		while (isValid()) {
			switch (getCurrentToken().type) {
				case TokenType::string:
					getHead()->add(new ValueNode(NodeType::string, NodeGroup::string, NodeShape::inBetween,
						getCurrentToken().value));
					skip();
					break;

				case TokenType::asterisk:
					pairFormatHead(NodeType::boldFormat);
					break;

				case TokenType::underline:
					pairFormatHead(NodeType::italicFormat);
					break;

				case TokenType::backtick:
					pairFormatHead(NodeType::monospaceFormat);
					break;

				case TokenType::newline: {
					size_t newlineSize = getCurrentToken().size;
					// getHead()->Add(new SizeNode(NodeType::newline, GetCurrent().size));
					skip();
					parseNewline(newlineSize);
					break;
				}

				case TokenType::anchor: {
					if (getHead()->group == NodeGroup::header) {
						static_cast<ValueNode*>(getHead())->value = getCurrentToken().value;
					}
					skip();
					break;
				}

				case TokenType::reverseTail:
					getHead()->add(new Node(NodeType::lineBreak, NodeGroup::break_));
					skip();
					break;

				case TokenType::openCurlyBracket:
					skip();
					switch (getCurrentToken().type) {
						case TokenType::colon:
							parseLink(NodeType::urlLink);
							break;

						case TokenType::exclamationMark:
							parseLink(NodeType::markerLink);
							break;

						case TokenType::caret:
							parseLink(NodeType::referenceLink);
							break;

						case TokenType::questionMark:
							parseLink(NodeType::inlineImage);
							break;

						default:
							break;
					}

					break;

				case TokenType::closeCurlyBracket:
					skip();
					popHead();
					break;

				case TokenType::pipe:
					while (headNodes.size() > 1 && headNodes.top()->type != NodeType::row)
						popHead();
					pushHead(new Node(NodeType::cell, NodeGroup::table));
					skip();
					break;

				case TokenType::hash:
					getHead()->add(new ValueNode(NodeType::hashtag, NodeGroup::tag, NodeShape::inBetween,
						getCurrentToken().value));
					skip();
					break;
				case TokenType::at:
					getHead()->add(new ValueNode(NodeType::username, NodeGroup::tag, NodeShape::inBetween,
						getCurrentToken().value));
					skip();
					break;

				default:
					skip();
					break;
			}
		}

		parseNewline();
	}

	void AstBuilder::reset() {
		tokenIndex = 0;
		tokenSize = lexer.getTokens().size();

		if (rootNode)
			delete rootNode;

		rootNode = new Node(NodeType::root);

		while (!headNodes.empty())
			headNodes.pop();

		headNodes.push(rootNode);
	}

	void AstBuilder::destroyTree() {
		traverseAndDestroyNode(rootNode);
	}

	std::string AstBuilder::getBuffer() {
		return lexer.getBuffer();
	}

	Node* AstBuilder::getTree() {
		return rootNode;
	}

	std::string AstBuilder::getTokensAsString() {
		return lexer.getTokensAsString();
	}

	std::string AstBuilder::getTreeAsString() {
		buffer.clear();

		traverseAndGenerateBuffer(getTree(), 0);

		return buffer + "\n";
	}

	void AstBuilder::traverseAndGenerateBuffer(Node* node, size_t depth) {
		for (size_t i = 0; i < depth; ++i)
			buffer += "    ";

		buffer += node->toString() + "\n";

		for (Node* child: node->children)
			traverseAndGenerateBuffer(child, depth + 1);
	}

	void AstBuilder::traverseAndDestroyNode(Node* node) {
		for (Node* child: node->children)
			traverseAndDestroyNode(child);

		if (node->group == NodeGroup::link) {
			Node* packageNode = static_cast<ContainerNode*>(node)->value;
			delete packageNode;
			packageNode = nullptr;

		}

		delete node;
		node = nullptr;
	}

	void AstBuilder::parseNewline(size_t newlineSize) {
		parseIndentation();

		switch (getCurrentToken().type) {
			case TokenType::tail:
				switch (getCurrentToken().size) {
					case 1:
						compareAndPopHead(NodeType::miniSection, newlineSize);
						pushHead(new ValueNode(NodeType::miniSection, NodeGroup::header, NodeShape::line));
						break;

					case 2:
						compareAndPopHead(NodeType::part, newlineSize);
						pushHead(new ValueNode(NodeType::part, NodeGroup::header, NodeShape::line));
						break;

					case 3:
						compareAndPopHead(NodeType::title, newlineSize);
						pushHead(new ValueNode(NodeType::title, NodeGroup::header, NodeShape::line));
						break;

					default:
						break;
				}
				skip();
				break;

			case TokenType::reverseTail:
				if (getCurrentToken().size == 2) {
					compareAndPopHead(NodeType::thematicBreak);
					getHead()->add(new Node(NodeType::thematicBreak, NodeGroup::break_, NodeShape::line));
					skip();
				} else if (getCurrentToken().size > 2) {
					compareAndPopHead(NodeType::pageBreak);
					getHead()->add(new Node(NodeType::pageBreak, NodeGroup::break_, NodeShape::line));
					skip();
				}
				break;

			case TokenType::arrow:
				switch (getCurrentToken().size) {
					case 1:
						compareAndPopHead(NodeType::subSubSection, newlineSize);
						pushHead(new ValueNode(NodeType::subSubSection, NodeGroup::header, NodeShape::line));
						break;

					case 2:
						compareAndPopHead(NodeType::subSection, newlineSize);
						pushHead(new ValueNode(NodeType::subSection, NodeGroup::header, NodeShape::line));
						break;

					case 3:
						compareAndPopHead(NodeType::section, newlineSize);
						pushHead(new ValueNode(NodeType::section, NodeGroup::header, NodeShape::line));
						break;

					case 4:
						compareAndPopHead(NodeType::chapter, newlineSize);
						pushHead(new ValueNode(NodeType::chapter, NodeGroup::header, NodeShape::line));
						break;

					default:
						break;
				}

				break;

			case TokenType::bullet:
				compareAndPopHead(NodeType::list);

				if (getHead()->type != NodeType::list)
					pushHead(new Node(NodeType::list, NodeGroup::list, NodeShape::block));

				pushHead(new Node(NodeType::item, NodeGroup::item, NodeShape::block));
				skip();
				break;

			case TokenType::numericBullet:
				compareAndPopHead(NodeType::numericList);

				if (getHead()->type != NodeType::numericList)
					pushHead(new Node(NodeType::numericList, NodeGroup::list, NodeShape::block));

				pushHead(new Node(NodeType::item, NodeGroup::item, NodeShape::block));
				skip();
				break;

			case TokenType::checkBox:
				compareAndPopHead(NodeType::checkList);

				if (getHead()->type != NodeType::checkList)
					pushHead(new Node(NodeType::checkList, NodeGroup::list, NodeShape::block));

				pushHead(new TernaryNode(NodeType::checkItem, NodeGroup::item, NodeShape::block,
					static_cast<TernaryState>(getCurrentToken().size)));
				skip();
				break;

			case TokenType::dollar:
				skip();
				parseBlock(getCurrentToken().type);
				break;

			case TokenType::line:
				while (headNodes.size() > 1 && headNodes.top()->shape != NodeShape::block)
					headNodes.pop();

				popHead();
				skip();
				break;

			default:
				if (getHead()->group != NodeGroup::table) {
					compareAndPopHead(NodeType::paragraph, newlineSize);

					if (getHead()->type != NodeType::paragraph)
						pushHead(new Node(NodeType::paragraph));
					else
						getHead()->add(new Node(NodeType::newline));
				} else {
					compareAndPopHead(NodeType::table);
					pushHead(new Node(NodeType::row, NodeGroup::table, NodeShape::line));
					pushHead(new Node(NodeType::cell, NodeGroup::table, NodeShape::line));
				}
				break;
		}
	}

	void AstBuilder::parseIndentation() {
		size_t currentDepth = 0;

		if (getCurrentToken().type == TokenType::space) {
			currentDepth = getCurrentToken().size / 4;
			skip();
		}

		if (currentDepth > depth) {
			size_t distance = currentDepth - depth - 1;
			for (size_t i = 0; i < distance; ++i)
				pushHead(new Node(NodeType::indent, NodeGroup::container, NodeShape::superBlock));

			pushHead(new Node(
				getHead()->group != NodeGroup::item ? NodeType::indent : NodeType::container,
				NodeGroup::container,
				NodeShape::superBlock));
		} else if (currentDepth < depth) {
			size_t distance = depth - currentDepth;
			size_t i = 0;
			while (i < distance) {
				if (getHead()->type == NodeType::root)
					break;

				if (getHead()->shape == NodeShape::superBlock)
					++i;

				popHead();
			}
		}

		depth = currentDepth;
	}

	void AstBuilder::parseBreak() {
		switch (getCurrentToken().size) {
			case 1:
				getHead()->add(new Node(NodeType::lineBreak, NodeGroup::break_));
				break;
			case 2:
				getHead()->add(new Node(NodeType::thematicBreak, NodeGroup::break_));
				break;
			case 3:
				getHead()->add(new Node(NodeType::pageBreak, NodeGroup::break_));
				break;
		}
		skip();
	}

	void AstBuilder::parseLink(NodeType type) {
		ContainerNode* container = new ContainerNode(type, NodeGroup::link);
		ValueNode* node = new ValueNode();
		skip();

		if (getCurrentToken().type == TokenType::string) {
			node->type = NodeType::string;
			node->value = getCurrentToken().value;
			skip();
		} else if (getCurrentToken().type == TokenType::symbol) {
			node->type = NodeType::symbol;
			node->value = getCurrentToken().value;
			skip();
		}
		container->value = node;

		if (getCurrentToken().type == TokenType::openCurlyBracket) {
			pushHead(container);
			pushHead(new Node(NodeType::container));
			skip();
		} else
			pushHead(container);
	}

	void AstBuilder::parseBlock(TokenType type) {
		switch (type) {
			case TokenType::tableKeyword:
				compareAndPopHead(NodeType::table);

				while (isValid() && getCurrentToken().type != TokenType::line) {
					skip();
				}
				pushHead(new TableNode());
				break;

			case TokenType::codeKeyword: {
				compareAndPopHead(NodeType::code);

				CodeNode* codeNode = new CodeNode();
				getHead()->add(codeNode);
				skip();
				if (getCurrentToken().type == TokenType::string) {
					codeNode->langCode = new ValueNode(NodeType::string, NodeGroup::string, NodeShape::block,
						getCurrentToken().value);
					skip();
				}

				// skip indentations
				if (getCurrentToken().type == TokenType::newline)
					skip();
				if (getCurrentToken().type == TokenType::space)
					skip();

				if (getCurrentToken().type == TokenType::line &&
					getNextToken(1).type == TokenType::string &&
					getNextToken(2).type == TokenType::line) {
					codeNode->value = getNextToken(1).value;
					skip(3);
				}
				break;
			}

			case TokenType::tocKeyword:
				while (isValid() && getCurrentToken().type != TokenType::newline)
					skip();
				getHead()->add(new Node(NodeType::toc, NodeGroup::unknown, NodeShape::line));
				break;

			case TokenType::indexKeyword:
				while (isValid() && getCurrentToken().type != TokenType::newline)
					skip();
				getHead()->add(new Node(NodeType::index, NodeGroup::unknown, NodeShape::line));
				break;

			case TokenType::referenceKeyword:
				while (isValid() && getCurrentToken().type != TokenType::newline)
					skip();
				getHead()->add(new Node(NodeType::reference, NodeGroup::unknown, NodeShape::line));
				break;

			case TokenType::fileKeyword:
				while (isValid() && getCurrentToken().type != TokenType::newline)
					skip();
				getHead()->add(new Node(NodeType::file, NodeGroup::unknown, NodeShape::line));
				break;

			case TokenType::imageKeyword:
				while (isValid() && getCurrentToken().type != TokenType::newline)
					skip();
				getHead()->add(new Node(NodeType::image, NodeGroup::unknown, NodeShape::line));
				break;

			case TokenType::admonitionKeyword:
				while (isValid() && getCurrentToken().type != TokenType::line)
					skip();
				getHead()->add(new Node(NodeType::admonition, NodeGroup::unknown, NodeShape::block));
				break;

//        	case TokenType::symbol:
//				while (isValid() && getCurrentToken().type != TokenType::line)
//                	skip();
//            	getHead()->add(new Node(NodeType::assignment, NodeGroup::unknown, NodeShape::line));
//            	break;

			default:
				break;
		}
	}

	Node* AstBuilder::getHead() {
		return headNodes.top();
	}

	void AstBuilder::pushHead(Node* node) {
		getHead()->add(node);
		headNodes.push(node);
	}

	void AstBuilder::compareAndPopHead(NodeType type) {
		if (headNodes.size() > 1) {
			if (headNodes.top()->type != type)
				while (headNodes.size() > 1 && headNodes.top()->type != type
					&& headNodes.top()->shape != NodeShape::superBlock)
					headNodes.pop();
			else if (headNodes.top()->shape == NodeShape::line)
				headNodes.pop();
		}
	}

	void AstBuilder::compareAndPopHead(NodeType type, size_t newlineSize) {
		if (headNodes.size() > 1) {
			if (headNodes.top()->type != type)
				while (headNodes.size() > 1 && headNodes.top()->type != type
					&& headNodes.top()->shape != NodeShape::superBlock)
					headNodes.pop();
			else if (newlineSize > 1 || headNodes.top()->shape == NodeShape::line)
				headNodes.pop();
		}

	}

	void AstBuilder::popHead() {
		if (headNodes.size() > 1)
			headNodes.pop();
	}

	void AstBuilder::pairFormatHead(NodeType type) {
		if (getHead()->type == type)
			popHead();
		else
			pushHead(new Node(type));

		skip();
	}

	bool AstBuilder::isValid() {
		return tokenIndex < tokenSize;
	}

	Token& AstBuilder::getCurrentToken() {
		return lexer.getTokenAt(tokenIndex);
	}

	Token& AstBuilder::getNextToken(size_t offset) {
		return tokenIndex + offset < tokenSize ? lexer.getTokenAt(tokenIndex + offset) : emptyToken;
	}

	void AstBuilder::skip(size_t offset) {
		tokenIndex += offset;
	}
}
