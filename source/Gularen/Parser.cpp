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
			static_cast<DocumentNode*>(node.get())->path = nextPath;
		}

		return node;
	}

	void Parser::parse() {
		lexer.tokenize();
		root = nullptr;

		if (lexer.get().empty()) return;

		index = 0;
		lastEnding = TokenType::newline;
		
		while (!scopes.empty()) {
			scopes.pop();
		}

		root = std::make_shared<DocumentNode>(pathVirtual ? "" : path);
		scopes.push(root);

		parseBlock();

		while (check(0)) {
			parseInline();
		}

		parseBlockEnding();
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

			case TokenType::hyphen:
				add(std::make_shared<PunctNode>(PunctType::hyphen, get(0).value));
				advance(0);
				break;
			case TokenType::enDash:
				add(std::make_shared<PunctNode>(PunctType::enDash, get(0).value));
				advance(0);
				break;
			case TokenType::emDash:
				add(std::make_shared<PunctNode>(PunctType::emDash, get(0).value));
				advance(0);
				break;

			case TokenType::emojiMarker:
				if (check(2) && is(1, TokenType::emojiCode) && is(2, TokenType::emojiMarker)) {
					add(std::make_shared<EmojiNode>(get(1).value));
					advance(2);
					break;
				}
				addText(get(0).value);
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

			case TokenType::headingIDOper:
				if (check(1) && is(1, TokenType::headingID) && getScope()->group == NodeGroup::heading) {
					HeadingNode* headingNode = static_cast<HeadingNode*>(getScope().get());
					if (headingNode->type == HeadingType::subtitle) {
						addText("> " + get(1).value);
						advance(1);
						break;
					}
					headingNode->id = get(1).value;
					advance(1);
					break;
				} 
				addText(">");
				advance(0);
				break;

			case TokenType::break_:
				if (get(0).value.size() == 2) {
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

			case TokenType::curlyOpen: {
				if (check(5) && is(1, TokenType::codeSource) && is(2, TokenType::curlyClose) &&
					is(3, TokenType::parenOpen) && is(4, TokenType::resourceLabel) && is(5, TokenType::parenClose)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::inline_;
					codeNode->lang = get(4).value;
					codeNode->source = get(1).value;
					add(codeNode);
					advance(5);
					break;
				}

				if (check(2) && is(1, TokenType::codeSource) && is(2, TokenType::curlyClose)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::inline_;
					codeNode->source = get(1).value;
					add(codeNode);
					advance(2);
					break;
				}

				addText("{");
				advance(0);
			}

			case TokenType::squareOpen: {
				std::shared_ptr<ResourceNode> linkNode = nullptr;

				if (check(2) && 
					is(1, TokenType::resource) && 
					is(2, TokenType::squareClose)) {
					linkNode = std::make_shared<ResourceNode>(
						hasNetProtocol(get(1).value) ? ResourceType::linker : ResourceType::linkerLocal, 
						get(1).value
					);
					add(linkNode);
					advance(2);
				} else if (check(4) && 
					is(1, TokenType::resource) && 
					is(2, TokenType::resourceIDMarker) && 
					is(3, TokenType::resourceID) && 
					is(4, TokenType::squareClose)) {
					linkNode = std::make_shared<ResourceNode>(ResourceType::linkerLocal, get(1).value);
					linkNode->id = get(3).value;
					add(linkNode);
					advance(4);
				}

				if (linkNode) {
					if (check(2) && is(0, TokenType::parenOpen) && is(1, TokenType::resourceLabel) && is(2, TokenType::parenClose)) {
						linkNode->label = get(1).value;
						advance(2);
					}
					break;
				}

				addText("[");
				advance(0);
				break;
			}

			case TokenType::presentMarker: {
				std::shared_ptr<ResourceNode> presentNode = nullptr;

				if (check(3) && 
					is(1, TokenType::squareOpen) && 
					is(2, TokenType::resource) && 
					is(3, TokenType::squareClose)) {
					presentNode = std::make_shared<ResourceNode>(
						hasNetProtocol(get(2).value) ? ResourceType::presenter : ResourceType::presenterLocal, 
						get(2).value
					);
					add(presentNode);
					advance(3);
				}

				if (presentNode) {
					if (check(2) && is(0, TokenType::parenOpen) && is(1, TokenType::resourceLabel) && is(2, TokenType::parenClose)) {
						presentNode->label = get(1).value;
						advance(2);
					}
					break;
				}

				addText("!");
				advance(0);
				break;
			}

			case TokenType::jumpMarker:
				if (check(3) && is(1, TokenType::squareOpen) && is(2, TokenType::jumpID) && is(3, TokenType::squareClose)) {
					add(std::make_shared<FootnoteJumpNode>(get(2).value));
					advance(3);
					break;
				}
				addText("^");
				advance(0);
				break;

			case TokenType::includeMarker:
				#ifndef __EMSCRIPTEN__
				if (check(3) && 
					is(1, TokenType::squareOpen) &&
					is(2, TokenType::resource) &&
					is(3, TokenType::squareClose) &&
					!hasNetProtocol(get(2).value)
					) {
					std::string directory = path == "" ? std::filesystem::current_path().string() : std::filesystem::path(path).parent_path().string();
					
					NodePtr fileNode = recursiveLoad(directory, get(2).value);
					add(fileNode ? fileNode : std::make_shared<DocumentNode>(get(2).value));
					advance(3);
					break;
				}
				#endif

				addText("?");
				advance(0);
				break;

			case TokenType::newline:
			case TokenType::newlinePlus:
			case TokenType::eof:
				lastEnding = get(0).type;
				advance(0);

				if (check(0)) {
					parseBlock();
				}
				break;

			case TokenType::comment:
				advance(0);
				break;
				
			default:
				addText(get(0).value);
				advance(0);
				break;
		}
	}

	void Parser::parseIndent() {
		switch (getScope()->group) {
			case NodeGroup::heading:
				if (getScope()->as<HeadingNode>().type == HeadingType::subtitle) {
					removeScope();
				}
				removeScope();
				break;

			default:
				break;
		}

		while (check(0) && (
			is(0, TokenType::indentIncr) || 
			is(0, TokenType::indentDecr) || 
			is(0, TokenType::bqIncr) || 
			is(0, TokenType::bqDecr)
			)) {

			if (is(0, TokenType::indentIncr)) {
				addScope(std::make_shared<IndentNode>());
				advance(0);
				continue;
			}

			if (is(0, TokenType::indentDecr)) {
				while (check(0) && scopes.size() > 1 && getScope()->group != NodeGroup::indent) {
					removeScope();
				}
				removeScope();
				advance(0);
				continue;
			}

			if (is(0, TokenType::bqIncr)) {
				addScope(std::make_shared<BQNode>());
				advance(0);
				continue;
			}

			if (is(0, TokenType::bqDecr)) {
				while (check(0) && scopes.size() > 1 && getScope()->group != NodeGroup::bq) {
					removeScope();
				}
				removeScope();
				advance(0);
				continue;
			}

			break;
		}
	}
	
	void Parser::parseBlockEnding() {
		switch (getScope()->group) {
			case NodeGroup::listItem: {
				removeScope();
				ListType type = getScope()->as<ListNode>().type;

				if (lastEnding == TokenType::newlinePlus || (
					check(0) && !(
						(is(0, TokenType::bullet)   && type == ListType::bullet) ||
						(is(0, TokenType::index)    && type == ListType::index) ||
						(is(0, TokenType::checkbox) && type == ListType::check) 
					)
				)) {
					lastListDeadBecauseNewlinePlus = lastEnding == TokenType::newlinePlus;
					removeScope();
				}
				break;
			}

			case NodeGroup::paragraph:
				if (getScope()->children.empty()) {
					removeScope();
					getScope()->children.pop_back();
				} else if (lastEnding == TokenType::newlinePlus || (check(0) &&
					is(0, TokenType::bullet) ||
					is(0, TokenType::index) ||
					is(0, TokenType::checkbox) ||
					is(0, TokenType::pipe) ||
					is(0, TokenType::chapterOper) ||
					is(0, TokenType::describeMarker)
				)) {
					removeScope();
				}
				break;
			
			case NodeGroup::tableCell:
				if (getScope()->children.empty()) {
					removeScope();
					getScope()->children.pop_back();
				} else {
					removeScope();
				}
				
				// tableRow
				removeScope();

				if (lastEnding == TokenType::newlinePlus || (check(0) && !is(0, TokenType::pipe))) {
					removeScope();
				}
				break;

			case NodeGroup::tableRow:
				removeScope();

				if (lastEnding == TokenType::newlinePlus || (check(0) && !is(0, TokenType::pipe))) {
					removeScope();
				}
				break;

			case NodeGroup::footnoteDescribe:
				removeScope();
				break;

			case NodeGroup::admon:
				removeScope();
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
					addScope(std::make_shared<ListNode>(ListType::bullet));
				}
				addScope(std::make_shared<ListItemNode>(getScope()->children.size() + 1));
				advance(0);
				break;

			case TokenType::index:
				if (getScope()->group != NodeGroup::list) {
					if (!getScope()->children.empty() && 
						getScope()->children.back()->group == NodeGroup::list &&
						getScope()->children.back()->as<ListNode>().type == ListType::index &&
						!lastListDeadBecauseNewlinePlus) {
						scopes.push(getScope()->children.back());
					} else {
						addScope(std::make_shared<ListNode>(ListType::index));
					}
				}
				addScope(std::make_shared<ListItemNode>(getScope()->children.size() + 1));
				advance(0);
				break;

			case TokenType::checkbox: {
				if (getScope()->group != NodeGroup::list) {
					if (!getScope()->children.empty() && 
						getScope()->children.back()->group == NodeGroup::list &&
						getScope()->children.back()->as<ListNode>().type == ListType::check &&
						!lastListDeadBecauseNewlinePlus) {
						scopes.push(getScope()->children.back());
					} else {
						addScope(std::make_shared<ListNode>(ListType::check));
					}
				}
				
				ListItemState state = ListItemState::none;
				switch (get(0).value[1]) {
					case ' ': state = ListItemState::todo; break;
					case 'v': state = ListItemState::done; break;
					case 'x': state = ListItemState::canceled; break;
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
					addScope(std::make_shared<TableRowNode>());
				}
				break;

			case TokenType::curlyOpen: {
				// see default inline
				break;
			}

			case TokenType::codeMarker: {
				if (check(2) && is(1, TokenType::codeSource) && is(2, TokenType::codeMarker)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::block;
					codeNode->source = get(1).value;
					add(codeNode);
					advance(2);
					break;
				}

				if (check(3) && is(1, TokenType::codeLang) && is(2, TokenType::codeSource) && is(3, TokenType::codeMarker)) {
					std::shared_ptr<CodeNode> codeNode = std::make_shared<CodeNode>();
					codeNode->type = CodeType::block;
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
				if (check(3) && is(1, TokenType::squareOpen) && is(2, TokenType::jumpID) && is(3, TokenType::squareClose)) {
					addScope(std::make_shared<FootnoteDescribeNode>(get(2).value));
					advance(3);
				}

				// see default inline
				break;

			case TokenType::admonNote:
				addScope(std::make_shared<AdmonNode>(AdmonType::note));
				advance(0);
				break;

			case TokenType::admonHint:
				addScope(std::make_shared<AdmonNode>(AdmonType::hint));
				advance(0);
				break;

			case TokenType::admonImportant:
				addScope(std::make_shared<AdmonNode>(AdmonType::important));
				advance(0);
				break;

			case TokenType::admonWarning:
				addScope(std::make_shared<AdmonNode>(AdmonType::warning));
				advance(0);
				break;

			case TokenType::admonDanger:
				addScope(std::make_shared<AdmonNode>(AdmonType::danger));
				advance(0);
				break;

			case TokenType::admonSeeAlso:
				addScope(std::make_shared<AdmonNode>(AdmonType::seeAlso));
				advance(0);
				break;

			case TokenType::chapterOper:
				addScope(std::make_shared<HeadingNode>(HeadingType::chapter));
				advance(0);
				break;

			case TokenType::sectionOper:
				addScope(std::make_shared<HeadingNode>(HeadingType::section));
				advance(0);
				break;

			case TokenType::subsectionOper:
				if (!getScope()->children.empty() && getScope()->children.back()->group == NodeGroup::heading && lastEnding == TokenType::newline) {
					scopes.push(getScope()->children.back());
					addScope(std::make_shared<HeadingNode>(HeadingType::subtitle));
				} else {
					addScope(std::make_shared<HeadingNode>(HeadingType::subsection));
				}
				advance(0);
				break;


			default:
				if (getScope()->group != NodeGroup::paragraph) {
					addScope(std::make_shared<ParagraphNode>());
				} else if (getScope()->group == NodeGroup::paragraph) {
					addText("\n");
				}
				break;
		}
	}
}
