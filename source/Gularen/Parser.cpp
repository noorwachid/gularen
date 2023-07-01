#include <Gularen/Parser.h>
#include <iostream>
#include <fstream>

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


	void Parser::addText(const std::string& value) {
		if (!scopes.top()->children.empty() && scopes.top()->children.back()->group == NodeGroup::text) {
			Node* node = scopes.top()->children.back().get();
			static_cast<TextNode*>(node)->value += value;
		} else {
			add(std::make_shared<TextNode>(value));
		}
	}

	void Parser::addScope(const NodePtr& node) {
		add(node);
		scopes.push(node);
	}

	void Parser::removeScope() {
		if (scopes.size() > 1)
			scopes.pop();
	}

	const NodePtr& Parser::getScope() {
		return scopes.top();
	}

	void Parser::set(const std::string& content, const std::string& path) {
		lexer.set(content);
		this->pathVirtual = true;
		this->path = path;
		this->previousPaths.clear();
		this->previousPaths.push_back(path);
	}

	void Parser::load(const std::string& path) {
		this->pathVirtual = false;
		this->path = path;
		this->previousPaths.clear();
		this->previousPaths.push_back(path);

		if (std::filesystem::is_directory(path)) {
			lexer.set("");
			return;
		}

		std::ifstream file(path);

		if (!file.is_open()) {
			lexer.set("");
			return;
		}

		std::string content;
		file.seekg(0, std::ios::end);   
		content.reserve(file.tellg());
		file.seekg(0, std::ios::beg);
		content.assign(
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
		);
		
		lexer.set(content);
	}

	NodePtr Parser::recursiveLoad(const std::string& directory, const std::string& nextPath) {
		std::string nextFullPath = directory + "/" + nextPath;
		for (const std::string& previousPath : previousPaths) {
			if (previousPath == nextFullPath) {
				return nullptr;
			}
		}
		
		Parser parser;
		parser.load(nextFullPath);
		parser.previousPaths.insert(parser.previousPaths.end(), previousPaths.begin(), previousPaths.end());
		parser.parse();

		NodePtr node = parser.get();
		
		if (node) {
			static_cast<FileNode*>(node.get())->path = nextPath;
		}

		return node;
	}

	void Parser::parse() {
		lexer.parse();
		root = nullptr;

		if (lexer.get().empty()) return;

		index = 0;
		lastNewline = 0;
		lastIndent = 0;
		
		while (!scopes.empty()) {
			scopes.pop();
		}

		root = std::make_shared<FileNode>(pathVirtual ? "" : path);
		scopes.push(root);

		parseBlock();

		while (check(0)) {
			parseInline();
		}
	}

	const NodePtr& Parser::get() const {
		return root;
	}

	void Parser::visit(const Visitor& visitor) {
		visit(visitor, root);
	}

	void Parser::visit(const Visitor& visitor, const NodePtr& node) {
		if (!visitor || !node) return;

		visitor(node);
		
		for (const NodePtr& childNode : node->children) {
			visit(visitor, childNode);
		}
	}

	void Parser::parseInline() {
		switch (get(0).type) {
			case TokenType::text:
				addText(get(0).value);
				advance(0);
				break;

			case TokenType::lsQuo:
				add(std::make_shared<PunctNode>(PunctType::lsQuo, get(0).value));
				advance(0);
				break;
			case TokenType::rsQuo:
				add(std::make_shared<PunctNode>(PunctType::rsQuo, get(0).value));
				advance(0);
				break;
			case TokenType::ldQuo:
				add(std::make_shared<PunctNode>(PunctType::ldQuo, get(0).value));
				advance(0);
				break;
			case TokenType::rdQuo:
				add(std::make_shared<PunctNode>(PunctType::rdQuo, get(0).value));
				advance(0);
				break;

			case TokenType::minus:
				add(std::make_shared<PunctNode>(PunctType::minus, get(0).value));
				advance(0);
				break;
			case TokenType::hyphen:
				add(std::make_shared<PunctNode>(PunctType::hyphen, get(0).value));
				advance(0);
				break;
			case TokenType::enDash:
				add(std::make_shared<PunctNode>(PunctType::enDash, get(0).value));
				advance(0);
				break;

			case TokenType::emoji:
				add(std::make_shared<EmojiNode>(get(0).value));
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
				addText(">");
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

			case TokenType::resource: {
				ResourceType type = hasNetProtocol(get(0).value) ? ResourceType::linker : ResourceType::linkerLocal;
				std::shared_ptr<ResourceNode> linkNode = std::make_shared<ResourceNode>(type, get(0).value);
				add(linkNode);
				advance(0);

				if (check(0) && is(0, TokenType::resourceID)) {
					linkNode->id = get(0).value;
					advance(0);
				}

				if (check(0) && is(0, TokenType::resourceLabel)) {
					linkNode->label = get(0).value;
					advance(0);
				}
				break;
			}

			case TokenType::presentMarker: {
				advance(0);

				if (!check(0) || !is(0, TokenType::resource)) {
					break;
				}

				ResourceType type = hasNetProtocol(get(0).value) ? ResourceType::presenter : ResourceType::presenterLocal;
				std::shared_ptr<ResourceNode> linkNode = std::make_shared<ResourceNode>(type, get(0).value);
				add(linkNode);
				advance(0);

				if (check(0) && is(0, TokenType::resourceID)) {
					linkNode->id = get(0).value;
					advance(0);
				}

				if (check(0) && is(0, TokenType::resourceLabel)) {
					linkNode->label = get(0).value;
					advance(0);
				}
				break;
			}

			case TokenType::jumpMarker:
				if (check(1) && is(1, TokenType::resource)) {
					add(std::make_shared<FootnoteJumpNode>(get(1).value));
					advance(1);
					break;
				}
				// see default inline
				break;

			case TokenType::includeMarker:
				if (check(1) && is(1, TokenType::resource)) {
					std::string directory = path == "" ? std::filesystem::current_path().string() : std::filesystem::path(path).parent_path().string();
					NodePtr fileNode = recursiveLoad(directory, get(1).value);
					add(fileNode ? fileNode : std::make_shared<FileNode>(get(1).value));
					advance(1);
					break;
				}
				// see default inline
				break;

			case TokenType::newline:
				lastNewline = 0;
				while (check(0) && is(0, TokenType::newline)) {
					lastNewline += get(0).count;
					advance(0);
				}
				if (check(0)) {
					parseBlock();
				}
				break;
				
			default:
				addText(get(0).value);
				advance(0);
				break;
		}
	}

	void Parser::parseBlock() {
		switch (getScope()->group) {
			case NodeGroup::listItem:
				lastScope = getScope();
				removeScope();

				if (lastNewline > 1 || 
					!(is(0, TokenType::bullet) || is(0, TokenType::index) || is(0, TokenType::checkbox)) ||
					is(0, TokenType::pipe) ||
					is(0, TokenType::headingMarker)
					) {
					removeScope();
					lastScope = nullptr;
				}
				break;

			case NodeGroup::heading:
				lastScope = getScope();
				removeScope();
				break;

			case NodeGroup::paragraph:
				lastScope = nullptr;
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
				lastScope = nullptr;
				removeScope();

				if (lastNewline > 1 || 
					is(0, TokenType::bullet) ||
					is(0, TokenType::index) ||
					is(0, TokenType::checkbox) ||
					!is(0, TokenType::pipe) ||
					is(0, TokenType::headingMarker)
					) {
					removeScope();
				}
				break;

			case NodeGroup::footnoteDescribe:
				lastScope = getScope();
				removeScope();
				break;

			case NodeGroup::admon:
				lastScope = getScope();
				removeScope();
				break;
			
			default:
				lastScope = nullptr;
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
				addScope(std::make_shared<ListItemNode>(getScope()->children.size() + 1));
				advance(0);
				break;

			case TokenType::index:
				if (getScope()->group != NodeGroup::list) {
					addScope(std::make_shared<ListNode>(ListType::index));
				}
				addScope(std::make_shared<ListItemNode>(getScope()->children.size() + 1));
				advance(0);
				break;

			case TokenType::checkbox: {
				if (getScope()->group != NodeGroup::list) {
					addScope(std::make_shared<ListNode>(ListType::check));
				}
				
				ListItemState state = ListItemState::none;
				switch (get(0).count) {
					case 0: state = ListItemState::todo; break;
					case 1: state = ListItemState::done; break;
					case 2: state = ListItemState::canceled; break;
				}
				
				addScope(std::make_shared<ListItemNode>(getScope()->children.size() + 1, state));
				advance(0);
				break;
			}

			case TokenType::pipe:
				if (getScope()->group != NodeGroup::table) {
					addScope(std::make_shared<TableNode>());
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

			case TokenType::codeMarker: {
				if (check(2) && is(1, TokenType::codeSource) && is(2, TokenType::codeMarker)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->source = get(1).value;
					add(codeNode);
					advance(2);
					break;
				}
				if (check(3) && is(1, TokenType::codeLang) && is(2, TokenType::codeSource) && is(3, TokenType::codeMarker)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->lang = get(1).value;
					codeNode->source = get(2).value;
					add(codeNode);
					advance(3);
					break;
				}

				// see default inline 
				break;
			}

			case TokenType::describeMarker:
				if (check(1) && is(1, TokenType::resource)) {
					addScope(std::make_shared<FootnoteDescribeNode>(get(1).value));
					advance(1);
				}

				// see default inline
				break;

			case TokenType::admon: {
				AdmonType type = AdmonType::note;
				switch (get(0).count) {
					case 1: type = AdmonType::note; break;
					case 2: type = AdmonType::hint; break;
					case 3: type = AdmonType::important; break;
					case 4: type = AdmonType::warning; break;
					case 5: type = AdmonType::danger; break;
					case 6: type = AdmonType::seeAlso; break;
					default: break;
				}
				addScope(std::make_shared<AdmonNode>(type));
				advance(0);
				break;
			}

			default:
				if (getScope()->group != NodeGroup::paragraph) {
					addScope(std::make_shared<ParagraphNode>());
				} else {
					addText("\n");
				}
				break;
		}
	}

	void Parser::parseIndent(size_t indent) {
		if (lastIndent > indent) {
			while (scopes.size() > 1 && lastIndent + 1 > indent) {
				if (getScope()->group == NodeGroup::indent) {
					if (lastIndent > 0) --lastIndent;
				}
				removeScope();
			}
			lastIndent = indent;
		} else if (lastIndent < indent) {
			if (lastIndent + 1 == indent) {
				// list is guarantied to have one listItem
				if (lastScope && (
					lastScope->group == NodeGroup::listItem ||
					lastScope->group == NodeGroup::footnoteDescribe ||
					lastScope->group == NodeGroup::admon
					)) {
					// NOTE: not using helper addScope to avoid circular reference,
					//       it took me 2 days to find the bug
					scopes.push(lastScope);
				}
			}

			while (lastIndent < indent) {
				++lastIndent;
				addScope(std::make_shared<IndentNode>());
			}
		}
	}
}
