#include "Gularen/Internal/Parser.h"
#include <fstream>
#include <iostream>

namespace Gularen {
	bool hasNetProtocol(const std::string& reference) {
		if (reference.empty())
			return false;

		if (reference[0] >= 'a' && reference[0] <= 'z') {
			size_t i = 0;
			size_t size = 0;
			while (i < reference.size() && reference[i] >= 'a' && reference[i] <= 'z') {
				++i;
				++size;
			}

			if (i + 2 < reference.size() && reference[i] == ':' && reference[i + 1] == '/' && reference[i + 2] == '/') {
				return true;
			}

			return false;
		}

		return false;
	}

	bool Parser::checkBoundary(size_t offset) const {
		return _index + offset < _lexer.getTokens().size();
	}

	bool Parser::isToken(size_t offset, TokenType type) const {
		return _lexer.getTokens()[_index + offset].type == type;
	}

	const Token& Parser::getToken(size_t offset) const {
		return _lexer.getTokens()[_index + offset];
	}

	void Parser::advance(size_t offset) {
		_index += 1 + offset;
	}

	void Parser::add(const NodePtr& node, const Range& range) {
		node->range = range;
		_scopes.top()->children.push_back(node);
	}

	void Parser::addText(const std::string& value, const Range& range) {
		if (!_scopes.top()->children.empty() && _scopes.top()->children.back()->group == NodeGroup::text) {
			Node* node = _scopes.top()->children.back().get();
			static_cast<TextNode*>(node)->value += value;
		} else {
			add(std::make_shared<TextNode>(value), range);
		}
	}

	void Parser::addScope(const NodePtr& node, const Range& range) {
		add(node, range);
		_scopes.push(node);
	}

	void Parser::removeScope(const Range& range) {
		if (_scopes.size() > 1)
			_scopes.pop();
	}

	const NodePtr& Parser::getScope() {
		return _scopes.top();
	}

	void Parser::set(const std::string& content, const std::string& path) {
		_lexer.set(content);
		_pathVirtual = true;
		_path = path;
		_previousPaths.clear();
		_previousPaths.push_back(path);
	}

	void Parser::load(const std::string& path) {
		_pathVirtual = false;
		_path = path;
		_previousPaths.clear();
		_previousPaths.push_back(path);

		if (std::filesystem::is_directory(path)) {
			_lexer.set("");
			return;
		}

		std::ifstream file(path);

		if (!file.is_open()) {
			_lexer.set("");
			return;
		}

		std::string content;
		file.seekg(0, std::ios::end);
		content.reserve(file.tellg());
		file.seekg(0, std::ios::beg);
		content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

		_lexer.set(content);
	}

	NodePtr Parser::recursiveLoad(const std::string& directory, const std::string& nextPath) {
		std::string nextFullPath = directory + "/" + nextPath;
		for (const std::string& previousPath : _previousPaths) {
			if (previousPath == nextFullPath) {
				return nullptr;
			}
		}

		Parser parser;
		parser.load(nextFullPath);
		parser._previousPaths.insert(parser._previousPaths.end(), _previousPaths.begin(), _previousPaths.end());
		parser.parse();

		NodePtr node = parser.getRoot();

		if (node) {
			static_cast<DocumentNode*>(node.get())->path = nextPath;
		}

		return node;
	}

	void Parser::parse() {
		_lexer.parse();
		_root = nullptr;

		if (_lexer.getTokens().empty())
			return;

		_index = 0;
		_lastEnding = TokenType::newline;

		while (!_scopes.empty()) {
			_scopes.pop();
		}

		_root = std::make_shared<DocumentNode>(_pathVirtual ? "" : _path);
		_scopes.push(_root);

		parseBlock();

		while (checkBoundary(0)) {
			parseInline();
		}

		parseBlockEnding();
	}

	const NodePtr& Parser::getRoot() const {
		return _root;
	}

	void Parser::visit(const Visitor& visitor) {
		recursiveVisit(visitor, _root);
	}

	void Parser::recursiveVisit(const Visitor& visitor, const NodePtr& node) {
		if (!visitor || !node)
			return;

		visitor(node);

		for (const NodePtr& childNode : node->children) {
			recursiveVisit(visitor, childNode);
		}
	}

	void Parser::parseInline() {
		switch (getToken(0).type) {
			case TokenType::text:
				addText(getToken(0).value, getToken(0).range);
				advance(0);
				break;

			case TokenType::singleQuoteOpen:
				add(std::make_shared<PunctNode>(PunctType::singleQuoteOpen, getToken(0).value), getToken(0).range);
				advance(0);
				break;
			case TokenType::singleQuoteClose:
				add(std::make_shared<PunctNode>(PunctType::singleQuoteClose, getToken(0).value), getToken(0).range);
				advance(0);
				break;
			case TokenType::quoteOpen:
				add(std::make_shared<PunctNode>(PunctType::quoteOpen, getToken(0).value), getToken(0).range);
				advance(0);
				break;
			case TokenType::quoteClose:
				add(std::make_shared<PunctNode>(PunctType::quoteClose, getToken(0).value), getToken(0).range);
				advance(0);
				break;

			case TokenType::hyphen:
				add(std::make_shared<PunctNode>(PunctType::hyphen, getToken(0).value), getToken(0).range);
				advance(0);
				break;
			case TokenType::enDash:
				add(std::make_shared<PunctNode>(PunctType::enDash, getToken(0).value), getToken(0).range);
				advance(0);
				break;
			case TokenType::emDash:
				add(std::make_shared<PunctNode>(PunctType::emDash, getToken(0).value), getToken(0).range);
				advance(0);
				break;

			case TokenType::emojiMark:
				if (checkBoundary(2) && isToken(1, TokenType::emojiCode) && isToken(2, TokenType::emojiMark)) {
					add(std::make_shared<EmojiNode>(getToken(1).value), getToken(0).range);
					advance(2);
					break;
				}
				addText(getToken(0).value, getToken(0).range);
				break;

			case TokenType::fsBold:
				if (getScope()->group == NodeGroup::fs &&
					static_cast<FSNode*>(getScope().get())->type == FSType::bold) {
					removeScope(getToken(0).range);
					advance(0);
					break;
				}
				addScope(std::make_shared<FSNode>(FSType::bold), getToken(0).range);
				advance(0);
				break;

			case TokenType::fsItalic:
				if (getScope()->group == NodeGroup::fs &&
					static_cast<FSNode*>(getScope().get())->type == FSType::italic) {
					removeScope(getToken(0).range);
					advance(0);
					break;
				}
				addScope(std::make_shared<FSNode>(FSType::italic), getToken(0).range);
				advance(0);
				break;

			case TokenType::fsMonospace:
				if (getScope()->group == NodeGroup::fs &&
					static_cast<FSNode*>(getScope().get())->type == FSType::monospace) {
					removeScope(getToken(0).range);
					advance(0);
					break;
				}
				addScope(std::make_shared<FSNode>(FSType::monospace), getToken(0).range);
				advance(0);
				break;

			case TokenType::headingIDMark:
				if (checkBoundary(1) && isToken(1, TokenType::headingID) && getScope()->group == NodeGroup::heading) {
					HeadingNode* headingNode = static_cast<HeadingNode*>(getScope().get());
					if (headingNode->type == HeadingType::subtitle) {
						addText("> " + getToken(1).value, getToken(0).range);
						advance(1);
						break;
					}
					headingNode->id = getToken(1).value;
					advance(1);
					break;
				}
				addText(">", getToken(0).range);
				advance(0);
				break;

			case TokenType::date: {
				std::shared_ptr<DateTimeNode> dateTimeNode = std::make_shared<DateTimeNode>();
				dateTimeNode->date = getToken(0).value;
				add(dateTimeNode, getToken(0).range);
				advance(0);
				break;
			}

			case TokenType::time: {
				std::shared_ptr<DateTimeNode> dateTimeNode = std::make_shared<DateTimeNode>();
				dateTimeNode->time = getToken(0).value;
				add(dateTimeNode, getToken(0).range);
				advance(0);
				break;
			}

			case TokenType::dateTime: {
				std::shared_ptr<DateTimeNode> dateTimeNode = std::make_shared<DateTimeNode>();
				dateTimeNode->date = getToken(0).value.substr(0, 10);
				dateTimeNode->time = getToken(0).value.substr(11);
				add(dateTimeNode, getToken(0).range);
				advance(0);
				break;
			}

			case TokenType::break_:
				add(std::make_shared<BreakNode>(BreakType::line), getToken(0).range);
				advance(0);
				break;

			case TokenType::pipe:
				if (getScope()->group == NodeGroup::tableCell) {
					removeScope(getToken(0).range);
				}

				if (checkBoundary(1) && !isToken(1, TokenType::newline) && getScope()->group == NodeGroup::tableRow) {
					addScope(std::make_shared<TableCellNode>(), getToken(0).range);
				}
				advance(0);
				break;

			case TokenType::curlyOpen: {
				if (checkBoundary(5) && isToken(1, TokenType::codeSource) && isToken(2, TokenType::curlyClose) &&
					isToken(3, TokenType::parenOpen) && isToken(4, TokenType::resourceLabel) &&
					isToken(5, TokenType::parenClose)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::inline_;
					codeNode->lang = getToken(4).value;
					codeNode->source = getToken(1).value;
					add(codeNode, getToken(0).range);
					advance(5);
					break;
				}

				if (checkBoundary(2) && isToken(1, TokenType::codeSource) && isToken(2, TokenType::curlyClose)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::inline_;
					codeNode->source = getToken(1).value;
					add(codeNode, getToken(0).range);
					advance(2);
					break;
				}

				addText("{", getToken(0).range);
				advance(0);
			}

			case TokenType::squareOpen: {
				std::shared_ptr<ResourceNode> linkNode = nullptr;

				if (checkBoundary(2) && isToken(1, TokenType::resource) && isToken(2, TokenType::squareClose)) {
					linkNode = std::make_shared<ResourceNode>(
						hasNetProtocol(getToken(1).value) ? ResourceType::link : ResourceType::linkLocal,
						getToken(1).value
					);
					add(linkNode, getToken(0).range);
					advance(2);
				} else if (checkBoundary(4) && isToken(1, TokenType::resource) && isToken(2, TokenType::resourceIDMark) && isToken(3, TokenType::resourceID) && isToken(4, TokenType::squareClose)) {
					linkNode = std::make_shared<ResourceNode>(ResourceType::linkLocal, getToken(1).value);
					linkNode->id = getToken(3).value;
					add(linkNode, getToken(0).range);
					advance(4);
				}

				if (linkNode) {
					if (checkBoundary(2) && isToken(0, TokenType::parenOpen) && isToken(1, TokenType::resourceLabel) &&
						isToken(2, TokenType::parenClose)) {
						linkNode->label = getToken(1).value;
						advance(2);
					}
					break;
				}

				addText("[", getToken(0).range);
				advance(0);
				break;
			}

			case TokenType::presentMark: {
				std::shared_ptr<ResourceNode> presentNode = nullptr;

				if (checkBoundary(3) && isToken(1, TokenType::squareOpen) && isToken(2, TokenType::resource) &&
					isToken(3, TokenType::squareClose)) {
					presentNode = std::make_shared<ResourceNode>(
						hasNetProtocol(getToken(2).value) ? ResourceType::present : ResourceType::presentLocal,
						getToken(2).value
					);
					add(presentNode, getToken(0).range);
					advance(3);
				}

				if (presentNode) {
					if (checkBoundary(2) && isToken(0, TokenType::parenOpen) && isToken(1, TokenType::resourceLabel) &&
						isToken(2, TokenType::parenClose)) {
						presentNode->label = getToken(1).value;
						advance(2);
					}
					break;
				}

				addText("!", getToken(0).range);
				advance(0);
				break;
			}

			case TokenType::JumpMark:
				if (checkBoundary(3) && isToken(1, TokenType::squareOpen) && isToken(2, TokenType::JumpID) &&
					isToken(3, TokenType::squareClose)) {
					add(std::make_shared<FootnoteJumpNode>(getToken(2).value), getToken(0).range);
					advance(3);
					break;
				}
				addText("^", getToken(0).range);
				advance(0);
				break;

			case TokenType::IncludeMark:
#ifndef __EMSCRIPTEN__
				if (checkBoundary(3) && isToken(1, TokenType::squareOpen) && isToken(2, TokenType::resource) &&
					isToken(3, TokenType::squareClose) && !hasNetProtocol(getToken(2).value)) {
					std::string directory = _path.empty() ? std::filesystem::current_path().string()
														  : std::filesystem::path(_path).parent_path().string();

					NodePtr fileNode = recursiveLoad(directory, getToken(2).value);
					add(fileNode ? fileNode : std::make_shared<DocumentNode>(getToken(2).value), getToken(0).range);
					advance(3);
					break;
				}
#endif

				addText("?", getToken(0).range);
				advance(0);
				break;

			case TokenType::newline:
			case TokenType::newlinePlus:
			case TokenType::end:
				_lastEnding = getToken(0).type;
				advance(0);

				if (checkBoundary(0)) {
					parseBlock();
				}
				break;

			case TokenType::comment:
				add(std::make_shared<CommentNode>(getToken(0).value.substr(1)), getToken(0).range);
				advance(0);
				break;

			default:
				addText(getToken(0).value, getToken(0).range);
				advance(0);
				break;
		}
	}

	void Parser::parseIndent() {
		switch (getScope()->group) {
			case NodeGroup::heading:
				if (getScope()->as<HeadingNode>().type == HeadingType::subtitle) {
					removeScope(getToken(0).range);
				}
				removeScope(getToken(0).range);
				break;

			default:
				break;
		}

		while (checkBoundary(0) && (isToken(0, TokenType::indentIncr) || isToken(0, TokenType::indentDecr) ||
									isToken(0, TokenType::bqIncr) || isToken(0, TokenType::bqDecr))) {

			if (isToken(0, TokenType::indentIncr)) {
				addScope(std::make_shared<IndentNode>(), getToken(0).range);
				advance(0);
				continue;
			}

			if (isToken(0, TokenType::indentDecr)) {
				while (checkBoundary(0) && _scopes.size() > 1 && getScope()->group != NodeGroup::indent) {
					removeScope(getToken(0).range);
				}
				IndentNode* indentNode = static_cast<IndentNode*>(getScope().get());
				removeScope(getToken(0).range);
				if (getScope()->group == NodeGroup::listItem) {
					indentNode->skipable = true;
				}
				advance(0);
				continue;
			}

			if (isToken(0, TokenType::bqIncr)) {
				addScope(std::make_shared<BQNode>(), getToken(0).range);
				advance(0);
				continue;
			}

			if (isToken(0, TokenType::bqDecr)) {
				while (checkBoundary(0) && _scopes.size() > 1 && getScope()->group != NodeGroup::bq) {
					removeScope(getToken(0).range);
				}
				removeScope(getToken(0).range);
				advance(0);
				continue;
			}

			break;
		}
	}

	void Parser::parseBlockEnding() {
		switch (getScope()->group) {
			case NodeGroup::listItem: {
				removeScope(getToken(0).range);
				ListType type = getScope()->as<ListNode>().type;

				if (_lastEnding == TokenType::newlinePlus ||
					(checkBoundary(0) && !((isToken(0, TokenType::bullet) && type == ListType::bullet) ||
										   (isToken(0, TokenType::index) && type == ListType::index) ||
										   (isToken(0, TokenType::checkbox) && type == ListType::check)))) {
					_lastListDeadBecauseNewlinePlus = _lastEnding == TokenType::newlinePlus;
					removeScope(getToken(0).range);
				}
				break;
			}

			case NodeGroup::paragraph:
				if (getScope()->children.empty()) {
					removeScope(getToken(0).range);
					getScope()->children.pop_back();
				} else if (_lastEnding == TokenType::newlinePlus || (checkBoundary(0) && isToken(0, TokenType::bullet) || isToken(0, TokenType::index) || isToken(0, TokenType::checkbox) || isToken(0, TokenType::pipe) || isToken(0, TokenType::chapterMark) || isToken(0, TokenType::describeMark))) {
					removeScope(getToken(0).range);
				}
				break;

			case NodeGroup::tableCell:
				if (getScope()->children.empty()) {
					removeScope(getToken(0).range);
					getScope()->children.pop_back();
				} else {
					removeScope(getToken(0).range);
				}

				// tableRow
				removeScope(getToken(0).range);

				if (_lastEnding == TokenType::newlinePlus || (checkBoundary(0) && !isToken(0, TokenType::pipe))) {
					removeScope(getToken(0).range);
				}
				break;

			case NodeGroup::tableRow:
				removeScope(getToken(0).range);

				if (_lastEnding == TokenType::newlinePlus || (checkBoundary(0) && !isToken(0, TokenType::pipe))) {
					removeScope(getToken(0).range);
				}
				break;

			case NodeGroup::footnoteDescribe:
				removeScope(getToken(0).range);
				break;

			case NodeGroup::admon:
				removeScope(getToken(0).range);
				break;

			default:
				break;
		}
	}

	void Parser::parseBlock() {
		parseIndent();
		parseBlockEnding();

		switch (getToken(0).type) {
			case TokenType::bullet:
				if (getScope()->group != NodeGroup::list) {
					addScope(std::make_shared<ListNode>(ListType::bullet), getToken(0).range);
				}
				addScope(std::make_shared<ListItemNode>(getScope()->children.size() + 1), getToken(0).range);
				advance(0);
				break;

			case TokenType::index:
				if (getScope()->group != NodeGroup::list) {
					if (!getScope()->children.empty() && getScope()->children.back()->group == NodeGroup::list &&
						getScope()->children.back()->as<ListNode>().type == ListType::index &&
						!_lastListDeadBecauseNewlinePlus) {
						_scopes.push(getScope()->children.back());
					} else {
						addScope(std::make_shared<ListNode>(ListType::index), getToken(0).range);
					}
				}
				addScope(std::make_shared<ListItemNode>(getScope()->children.size() + 1), getToken(0).range);
				advance(0);
				break;

			case TokenType::checkbox: {
				if (getScope()->group != NodeGroup::list) {
					if (!getScope()->children.empty() && getScope()->children.back()->group == NodeGroup::list &&
						getScope()->children.back()->as<ListNode>().type == ListType::check &&
						!_lastListDeadBecauseNewlinePlus) {
						_scopes.push(getScope()->children.back());
					} else {
						addScope(std::make_shared<ListNode>(ListType::check), getToken(0).range);
					}
				}

				ListItemState state = ListItemState::none;
				switch (getToken(0).value[1]) {
					case ' ':
						state = ListItemState::todo;
						break;
					case 'v':
						state = ListItemState::done;
						break;
					case 'x':
						state = ListItemState::cancelled;
						break;
				}

				addScope(std::make_shared<ListItemNode>(getScope()->children.size() + 1, state), getToken(0).range);
				advance(0);
				break;
			}

			case TokenType::pipe:
				if (getScope()->group != NodeGroup::table) {
					addScope(std::make_shared<TableNode>(), getToken(0).range);
				}

				if (checkBoundary(1) && isToken(1, TokenType::pipeConnector)) {
					TableNode* tableNode = static_cast<TableNode*>(getScope().get());
					if (tableNode->header == 0) {
						tableNode->header = tableNode->children.size();
						tableNode->footer = tableNode->children.size();
					} else {
						tableNode->footer = tableNode->children.size();
					}

					while (checkBoundary(0) && (isToken(0, TokenType::pipe) || isToken(0, TokenType::pipeConnector))) {
						if (isToken(0, TokenType::pipeConnector)) {
							if (getToken(0).value == ":--") {
								tableNode->alignments.push_back(Alignment::left);
							} else if (getToken(0).value == "--:") {
								tableNode->alignments.push_back(Alignment::right);
							} else {
								tableNode->alignments.push_back(Alignment::center);
							}
						}
						advance(0);
					}
					break;
				}

				if (getScope()->group == NodeGroup::table) {
					addScope(std::make_shared<TableRowNode>(), getToken(0).range);
				}
				break;

			case TokenType::curlyOpen: {
				// see default inline
				break;
			}

			case TokenType::codeMark: {
				if (checkBoundary(2) && isToken(1, TokenType::codeSource) && isToken(2, TokenType::codeMark)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::block;
					codeNode->source = getToken(1).value;
					add(codeNode, getToken(0).range);
					advance(2);
					break;
				}

				if (checkBoundary(3) && isToken(1, TokenType::codeLang) && isToken(2, TokenType::codeSource) &&
					isToken(3, TokenType::codeMark)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::block;
					codeNode->lang = getToken(1).value;
					codeNode->source = getToken(2).value;
					add(codeNode, getToken(0).range);
					advance(3);
					break;
				}

				// see default inline
				break;
			}

			case TokenType::describeMark:
				if (checkBoundary(3) && isToken(1, TokenType::squareOpen) && isToken(2, TokenType::JumpID) &&
					isToken(3, TokenType::squareClose)) {
					addScope(std::make_shared<FootnoteDescribeNode>(getToken(2).value), getToken(0).range);
					advance(3);
				}

				// see default inline
				break;

			case TokenType::admonNote:
				addScope(std::make_shared<AdmonNode>(AdmonType::note), getToken(0).range);
				advance(0);
				break;

			case TokenType::admonHint:
				addScope(std::make_shared<AdmonNode>(AdmonType::hint), getToken(0).range);
				advance(0);
				break;

			case TokenType::admonImportant:
				addScope(std::make_shared<AdmonNode>(AdmonType::important), getToken(0).range);
				advance(0);
				break;

			case TokenType::admonWarning:
				addScope(std::make_shared<AdmonNode>(AdmonType::warning), getToken(0).range);
				advance(0);
				break;

			case TokenType::admonSeeAlso:
				addScope(std::make_shared<AdmonNode>(AdmonType::seeAlso), getToken(0).range);
				advance(0);
				break;

			case TokenType::admonTip:
				addScope(std::make_shared<AdmonNode>(AdmonType::tip), getToken(0).range);
				advance(0);
				break;

			case TokenType::chapterMark:
				addScope(std::make_shared<HeadingNode>(HeadingType::chapter), getToken(0).range);
				advance(0);
				break;

			case TokenType::sectionMark:
				addScope(std::make_shared<HeadingNode>(HeadingType::section), getToken(0).range);
				advance(0);
				break;

			case TokenType::subsectionMark:
				if (!getScope()->children.empty() && getScope()->children.back()->group == NodeGroup::heading &&
					_lastEnding == TokenType::newline) {
					_scopes.push(getScope()->children.back());
					addScope(std::make_shared<HeadingNode>(HeadingType::subtitle), getToken(0).range);
				} else {
					addScope(std::make_shared<HeadingNode>(HeadingType::subsection), getToken(0).range);
				}
				advance(0);
				break;

			case TokenType::break_:
				if (getToken(0).value.size() == 3) {
					add(std::make_shared<BreakNode>(BreakType::page), getToken(0).range);
					advance(0);
					break;
				}
				// see inline break_
				break;

			case TokenType::dinkus:
				add(std::make_shared<BreakNode>(BreakType::thematic), getToken(0).range);
				advance(0);
				break;

			default:
				if (getScope()->group != NodeGroup::paragraph) {
					addScope(std::make_shared<ParagraphNode>(), getToken(0).range);
				} else if (getScope()->group == NodeGroup::paragraph) {
					addText("\n", getToken(0).range);
				}
				break;
		}
	}
}
