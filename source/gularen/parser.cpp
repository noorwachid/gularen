#include "gularen/parser.h"
#include <fstream>
#include <iostream>

namespace Gularen {
	bool HasNetProtocol(const std::string& reference) {
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

	bool Parser::check(size_t offset) const {
		return _index + offset < _lexer.get().size();
	}

	bool Parser::is(size_t offset, TokenType type) const {
		return _lexer.get()[_index + offset].type == type;
	}

	const Token& Parser::get(size_t offset) const {
		return _lexer.get()[_index + offset];
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
		this->_pathVirtual = true;
		this->_path = path;
		this->_previousPaths.clear();
		this->_previousPaths.push_back(path);
	}

	void Parser::load(const std::string& path) {
		this->_pathVirtual = false;
		this->_path = path;
		this->_previousPaths.clear();
		this->_previousPaths.push_back(path);

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

		NodePtr node = parser.get();

		if (node) {
			static_cast<DocumentNode*>(node.get())->path = nextPath;
		}

		return node;
	}

	void Parser::parse() {
		_lexer.tokenize();
		_root = nullptr;

		if (_lexer.get().empty())
			return;

		_index = 0;
		_lastEnding = TokenType::newline;

		while (!_scopes.empty()) {
			_scopes.pop();
		}

		_root = std::make_shared<DocumentNode>(_pathVirtual ? "" : _path);
		_scopes.push(_root);

		parseBlock();

		while (check(0)) {
			parseInline();
		}

		parseBlockEnding();
	}

	const NodePtr& Parser::get() const {
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
		switch (get(0).type) {
			case TokenType::text:
				addText(get(0).value, get(0).range);
				advance(0);
				break;

			case TokenType::singleQuoteOpen:
				add(std::make_shared<PunctNode>(PunctType::singleQuoteOpen, get(0).value), get(0).range);
				advance(0);
				break;
			case TokenType::singleQuoteClose:
				add(std::make_shared<PunctNode>(PunctType::singleQuoteClose, get(0).value), get(0).range);
				advance(0);
				break;
			case TokenType::quoteOpen:
				add(std::make_shared<PunctNode>(PunctType::quoteOpen, get(0).value), get(0).range);
				advance(0);
				break;
			case TokenType::quoteClose:
				add(std::make_shared<PunctNode>(PunctType::quoteClose, get(0).value), get(0).range);
				advance(0);
				break;

			case TokenType::hyphen:
				add(std::make_shared<PunctNode>(PunctType::hyphen, get(0).value), get(0).range);
				advance(0);
				break;
			case TokenType::enDash:
				add(std::make_shared<PunctNode>(PunctType::enDash, get(0).value), get(0).range);
				advance(0);
				break;
			case TokenType::emDash:
				add(std::make_shared<PunctNode>(PunctType::emDash, get(0).value), get(0).range);
				advance(0);
				break;

			case TokenType::emojiMark:
				if (check(2) && is(1, TokenType::emojiCode) && is(2, TokenType::emojiMark)) {
					add(std::make_shared<EmojiNode>(get(1).value), get(0).range);
					advance(2);
					break;
				}
				addText(get(0).value, get(0).range);
				break;

			case TokenType::fsBold:
				if (getScope()->group == NodeGroup::fs &&
					static_cast<FSNode*>(getScope().get())->type == FSType::bold) {
					removeScope(get(0).range);
					advance(0);
					break;
				}
				addScope(std::make_shared<FSNode>(FSType::bold), get(0).range);
				advance(0);
				break;

			case TokenType::fsItalic:
				if (getScope()->group == NodeGroup::fs &&
					static_cast<FSNode*>(getScope().get())->type == FSType::italic) {
					removeScope(get(0).range);
					advance(0);
					break;
				}
				addScope(std::make_shared<FSNode>(FSType::italic), get(0).range);
				advance(0);
				break;

			case TokenType::fsMonospace:
				if (getScope()->group == NodeGroup::fs &&
					static_cast<FSNode*>(getScope().get())->type == FSType::monospace) {
					removeScope(get(0).range);
					advance(0);
					break;
				}
				addScope(std::make_shared<FSNode>(FSType::monospace), get(0).range);
				advance(0);
				break;

			case TokenType::headingIDMark:
				if (check(1) && is(1, TokenType::headingID) && getScope()->group == NodeGroup::deading) {
					HeadingNode* headingNode = static_cast<HeadingNode*>(getScope().get());
					if (headingNode->type == HeadingType::subtitle) {
						addText("> " + get(1).value, get(0).range);
						advance(1);
						break;
					}
					headingNode->id = get(1).value;
					advance(1);
					break;
				}
				addText(">", get(0).range);
				advance(0);
				break;

			case TokenType::date: {
				std::shared_ptr<DateTimeNode> dateTimeNode = std::make_shared<DateTimeNode>();
				dateTimeNode->date = get(0).value;
				add(dateTimeNode, get(0).range);
				advance(0);
				break;
			}

			case TokenType::time: {
				std::shared_ptr<DateTimeNode> dateTimeNode = std::make_shared<DateTimeNode>();
				dateTimeNode->time = get(0).value;
				add(dateTimeNode, get(0).range);
				advance(0);
				break;
			}

			case TokenType::dateTime: {
				std::shared_ptr<DateTimeNode> dateTimeNode = std::make_shared<DateTimeNode>();
				dateTimeNode->date = get(0).value.substr(0, 10);
				dateTimeNode->time = get(0).value.substr(11);
				add(dateTimeNode, get(0).range);
				advance(0);
				break;
			}

			case TokenType::break_:
				add(std::make_shared<BreakNode>(BreakType::line), get(0).range);
				advance(0);
				break;

			case TokenType::pipe:
				if (getScope()->group == NodeGroup::tableCell) {
					removeScope(get(0).range);
				}

				if (check(1) && !is(1, TokenType::newline) && getScope()->group == NodeGroup::tableRow) {
					addScope(std::make_shared<TableCellNode>(), get(0).range);
				}
				advance(0);
				break;

			case TokenType::curlyOpen: {
				if (check(5) && is(1, TokenType::codeSource) && is(2, TokenType::curlyClose) &&
					is(3, TokenType::parenOpen) && is(4, TokenType::resourceLabel) && is(5, TokenType::parenClose)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::inline_;
					codeNode->lang = get(4).value;
					codeNode->source = get(1).value;
					add(codeNode, get(0).range);
					advance(5);
					break;
				}

				if (check(2) && is(1, TokenType::codeSource) && is(2, TokenType::curlyClose)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::inline_;
					codeNode->source = get(1).value;
					add(codeNode, get(0).range);
					advance(2);
					break;
				}

				addText("{", get(0).range);
				advance(0);
			}

			case TokenType::squareOpen: {
				std::shared_ptr<ResourceNode> linkNode = nullptr;

				if (check(2) && is(1, TokenType::resource) && is(2, TokenType::squareClose)) {
					linkNode = std::make_shared<ResourceNode>(
						HasNetProtocol(get(1).value) ? ResourceType::link : ResourceType::linkLocal, get(1).value
					);
					add(linkNode, get(0).range);
					advance(2);
				} else if (check(4) && is(1, TokenType::resource) && is(2, TokenType::resourceIDMark) && is(3, TokenType::resourceID) && is(4, TokenType::squareClose)) {
					linkNode = std::make_shared<ResourceNode>(ResourceType::linkLocal, get(1).value);
					linkNode->id = get(3).value;
					add(linkNode, get(0).range);
					advance(4);
				}

				if (linkNode) {
					if (check(2) && is(0, TokenType::parenOpen) && is(1, TokenType::resourceLabel) &&
						is(2, TokenType::parenClose)) {
						linkNode->label = get(1).value;
						advance(2);
					}
					break;
				}

				addText("[", get(0).range);
				advance(0);
				break;
			}

			case TokenType::presentMark: {
				std::shared_ptr<ResourceNode> presentNode = nullptr;

				if (check(3) && is(1, TokenType::squareOpen) && is(2, TokenType::resource) &&
					is(3, TokenType::squareClose)) {
					presentNode = std::make_shared<ResourceNode>(
						HasNetProtocol(get(2).value) ? ResourceType::present : ResourceType::presentLocal, get(2).value
					);
					add(presentNode, get(0).range);
					advance(3);
				}

				if (presentNode) {
					if (check(2) && is(0, TokenType::parenOpen) && is(1, TokenType::resourceLabel) &&
						is(2, TokenType::parenClose)) {
						presentNode->label = get(1).value;
						advance(2);
					}
					break;
				}

				addText("!", get(0).range);
				advance(0);
				break;
			}

			case TokenType::jumpMark:
				if (check(3) && is(1, TokenType::squareOpen) && is(2, TokenType::jumpID) &&
					is(3, TokenType::squareClose)) {
					add(std::make_shared<FootnoteJumpNode>(get(2).value), get(0).range);
					advance(3);
					break;
				}
				addText("^", get(0).range);
				advance(0);
				break;

			case TokenType::includeMark:
				#ifndef __EMSCRIPTEN__
				if (check(3) && is(1, TokenType::squareOpen) && is(2, TokenType::resource) &&
					is(3, TokenType::squareClose) && !HasNetProtocol(get(2).value)) {
					std::string directory = _path == "" ? std::filesystem::current_path().string()
														: std::filesystem::path(_path).parent_path().string();

					NodePtr fileNode = recursiveLoad(directory, get(2).value);
					add(fileNode ? fileNode : std::make_shared<DocumentNode>(get(2).value), get(0).range);
					advance(3);
					break;
				}
				#endif

				addText("?", get(0).range);
				advance(0);
				break;

			case TokenType::newline:
			case TokenType::newlinePlus:
			case TokenType::end:
				_lastEnding = get(0).type;
				advance(0);

				if (check(0)) {
					parseBlock();
				}
				break;

			case TokenType::comment:
				add(std::make_shared<CommentNode>(get(0).value.substr(1)), get(0).range);
				advance(0);
				break;

			default:
				addText(get(0).value, get(0).range);
				advance(0);
				break;
		}
	}

	void Parser::parseIndent() {
		switch (getScope()->group) {
			case NodeGroup::deading:
				if (getScope()->as<HeadingNode>().type == HeadingType::subtitle) {
					removeScope(get(0).range);
				}
				removeScope(get(0).range);
				break;

			default:
				break;
		}

		while (check(0) && (is(0, TokenType::indentIncr) || is(0, TokenType::indentDecr) || is(0, TokenType::bqIncr) ||
							is(0, TokenType::bqDecr))) {

			if (is(0, TokenType::indentIncr)) {
				addScope(std::make_shared<IndentNode>(), get(0).range);
				advance(0);
				continue;
			}

			if (is(0, TokenType::indentDecr)) {
				while (check(0) && _scopes.size() > 1 && getScope()->group != NodeGroup::indent) {
					removeScope(get(0).range);
				}
				removeScope(get(0).range);
				advance(0);
				continue;
			}

			if (is(0, TokenType::bqIncr)) {
				addScope(std::make_shared<BQNode>(), get(0).range);
				advance(0);
				continue;
			}

			if (is(0, TokenType::bqDecr)) {
				while (check(0) && _scopes.size() > 1 && getScope()->group != NodeGroup::bq) {
					removeScope(get(0).range);
				}
				removeScope(get(0).range);
				advance(0);
				continue;
			}

			break;
		}
	}

	void Parser::parseBlockEnding() {
		switch (getScope()->group) {
			case NodeGroup::listItem: {
				removeScope(get(0).range);
				ListType type = getScope()->as<ListNode>().type;

				if (_lastEnding == TokenType::newlinePlus ||
					(check(0) && !((is(0, TokenType::bullet) && type == ListType::bullet) ||
								   (is(0, TokenType::index) && type == ListType::index) ||
								   (is(0, TokenType::checkbox) && type == ListType::check)))) {
					_lastListDeadBecauseNewlinePlus = _lastEnding == TokenType::newlinePlus;
					removeScope(get(0).range);
				}
				break;
			}

			case NodeGroup::paragraph:
				if (getScope()->children.empty()) {
					removeScope(get(0).range);
					getScope()->children.pop_back();
				} else if (_lastEnding == TokenType::newlinePlus || (check(0) && is(0, TokenType::bullet) || is(0, TokenType::index) || is(0, TokenType::checkbox) || is(0, TokenType::pipe) || is(0, TokenType::chapterMark) || is(0, TokenType::describeMark))) {
					removeScope(get(0).range);
				}
				break;

			case NodeGroup::tableCell:
				if (getScope()->children.empty()) {
					removeScope(get(0).range);
					getScope()->children.pop_back();
				} else {
					removeScope(get(0).range);
				}

				// tableRow
				removeScope(get(0).range);

				if (_lastEnding == TokenType::newlinePlus || (check(0) && !is(0, TokenType::pipe))) {
					removeScope(get(0).range);
				}
				break;

			case NodeGroup::tableRow:
				removeScope(get(0).range);

				if (_lastEnding == TokenType::newlinePlus || (check(0) && !is(0, TokenType::pipe))) {
					removeScope(get(0).range);
				}
				break;

			case NodeGroup::footnoteDescribe:
				removeScope(get(0).range);
				break;

			case NodeGroup::admon:
				removeScope(get(0).range);
				break;

			default:
				break;
		}
	}

	void Parser::parseBlock() {
		parseIndent();
		parseBlockEnding();

		switch (get(0).type) {
			case TokenType::bullet:
				if (getScope()->group != NodeGroup::list) {
					addScope(std::make_shared<ListNode>(ListType::bullet), get(0).range);
				}
				addScope(std::make_shared<ListItemNode>(getScope()->children.size() + 1), get(0).range);
				advance(0);
				break;

			case TokenType::index:
				if (getScope()->group != NodeGroup::list) {
					if (!getScope()->children.empty() && getScope()->children.back()->group == NodeGroup::list &&
						getScope()->children.back()->as<ListNode>().type == ListType::index &&
						!_lastListDeadBecauseNewlinePlus) {
						_scopes.push(getScope()->children.back());
					} else {
						addScope(std::make_shared<ListNode>(ListType::index), get(0).range);
					}
				}
				addScope(std::make_shared<ListItemNode>(getScope()->children.size() + 1), get(0).range);
				advance(0);
				break;

			case TokenType::checkbox: {
				if (getScope()->group != NodeGroup::list) {
					if (!getScope()->children.empty() && getScope()->children.back()->group == NodeGroup::list &&
						getScope()->children.back()->as<ListNode>().type == ListType::check &&
						!_lastListDeadBecauseNewlinePlus) {
						_scopes.push(getScope()->children.back());
					} else {
						addScope(std::make_shared<ListNode>(ListType::check), get(0).range);
					}
				}

				ListItemState state = ListItemState::none;
				switch (get(0).value[1]) {
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

				addScope(std::make_shared<ListItemNode>(getScope()->children.size() + 1, state), get(0).range);
				advance(0);
				break;
			}

			case TokenType::pipe:
				if (getScope()->group != NodeGroup::table) {
					addScope(std::make_shared<TableNode>(), get(0).range);
				}

				if (check(1) && is(1, TokenType::pipeConnector)) {
					TableNode* tableNode = static_cast<TableNode*>(getScope().get());
					if (tableNode->header == 0) {
						tableNode->header = tableNode->children.size();
						tableNode->footer = tableNode->children.size();
					} else {
						tableNode->footer = tableNode->children.size();
					}

					while (check(0) && (is(0, TokenType::pipe) || is(0, TokenType::pipeConnector))) {
						if (is(0, TokenType::pipeConnector)) {
							if (get(0).value == ":--") {
								tableNode->alignments.push_back(Alignment::left);
							} else if (get(0).value == "--:") {
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
					addScope(std::make_shared<TableRowNode>(), get(0).range);
				}
				break;

			case TokenType::curlyOpen: {
				// see default inline
				break;
			}

			case TokenType::codeMark: {
				if (check(2) && is(1, TokenType::codeSource) && is(2, TokenType::codeMark)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::block;
					codeNode->source = get(1).value;
					add(codeNode, get(0).range);
					advance(2);
					break;
				}

				if (check(3) && is(1, TokenType::codeLang) && is(2, TokenType::codeSource) &&
					is(3, TokenType::codeMark)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::block;
					codeNode->lang = get(1).value;
					codeNode->source = get(2).value;
					add(codeNode, get(0).range);
					advance(3);
					break;
				}

				// see default inline
				break;
			}

			case TokenType::describeMark:
				if (check(3) && is(1, TokenType::squareOpen) && is(2, TokenType::jumpID) &&
					is(3, TokenType::squareClose)) {
					addScope(std::make_shared<FootnoteDescribeNode>(get(2).value), get(0).range);
					advance(3);
				}

				// see default inline
				break;

			case TokenType::admonNote:
				addScope(std::make_shared<AdmonNode>(AdmonType::note), get(0).range);
				advance(0);
				break;

			case TokenType::admonHint:
				addScope(std::make_shared<AdmonNode>(AdmonType::hint), get(0).range);
				advance(0);
				break;

			case TokenType::admonImportant:
				addScope(std::make_shared<AdmonNode>(AdmonType::important), get(0).range);
				advance(0);
				break;

			case TokenType::admonWarning:
				addScope(std::make_shared<AdmonNode>(AdmonType::warning), get(0).range);
				advance(0);
				break;

			case TokenType::admonSeeAlso:
				addScope(std::make_shared<AdmonNode>(AdmonType::seeAlso), get(0).range);
				advance(0);
				break;

			case TokenType::admonTip:
				addScope(std::make_shared<AdmonNode>(AdmonType::tip), get(0).range);
				advance(0);
				break;

			case TokenType::chapterMark:
				addScope(std::make_shared<HeadingNode>(HeadingType::chapter), get(0).range);
				advance(0);
				break;

			case TokenType::sectionMark:
				addScope(std::make_shared<HeadingNode>(HeadingType::section), get(0).range);
				advance(0);
				break;

			case TokenType::subsectionMark:
				if (!getScope()->children.empty() && getScope()->children.back()->group == NodeGroup::deading &&
					_lastEnding == TokenType::newline) {
					_scopes.push(getScope()->children.back());
					addScope(std::make_shared<HeadingNode>(HeadingType::subtitle), get(0).range);
				} else {
					addScope(std::make_shared<HeadingNode>(HeadingType::subsection), get(0).range);
				}
				advance(0);
				break;

			case TokenType::break_:
				if (get(0).value.size() == 3) {
					add(std::make_shared<BreakNode>(BreakType::page), get(0).range);
					advance(0);
					break;
				}
				// see inline break_
				break;

			case TokenType::dinkus:
				add(std::make_shared<BreakNode>(BreakType::thematic), get(0).range);
				advance(0);
				break;

			default:
				if (getScope()->group != NodeGroup::paragraph) {
					addScope(std::make_shared<ParagraphNode>(), get(0).range);
				} else if (getScope()->group == NodeGroup::paragraph) {
					addText("\n", get(0).range);
				}
				break;
		}
	}
}
