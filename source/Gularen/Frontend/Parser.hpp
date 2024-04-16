#pragma once

#include "Gularen/Frontend/Lexer.hpp"
#include "Gularen/Frontend/Node.hpp"
#include <fstream>

// TODO: allow double newline on blockquote 
// TODO: escaping with \

namespace Gularen {

class Parser {
public:
	Parser() {
		_document = nullptr;
		_fileInclusion = true;
		_error = false;
		_stopped = false;
	}

	~Parser() {
		delete _document;
		_document = nullptr;
	}

	Document* parseFile(std::string_view path) {
		_document = new Document();
		_document->path = path;

		if (_workspaceFolder.empty()) {
			for (size_t i = path.size(); i > 0; i -= 1) {
				if (path[i - 1] == '/') {
					_workspaceFolder = std::string(path.data(), i - 1);
					break;
				}
			}

			if (_workspaceFolder.size() == 0) {
				_workspaceFolder = ".";
			}
		}

		std::ifstream file(path);

		if (!file.is_open()) {
			delete _document;
			_document = nullptr;
			return nullptr;
		}

		_document->content.assign(std::filesystem::file_size(path), '\0');
		file.read(_document->content.data(), _document->content.size());

		return _parse(std::string_view(_document->content.data(), _document->content.size()));
	}

	Document* parse(std::string_view content) {
		_document = new Document();

		return _parse(content);
	}

	void setWorkspaceFolder(std::string_view path) {
		_workspaceFolder = path;
	}

	void setFileInclusion(bool state) {
		_fileInclusion = state;
	}

private:
	Document* _parse(std::string_view content) {
		_lexer.parse(content);
		_tokenIndex = 0;

		// for (size_t i = 0; i < _lexer.size(); i += 1) {
		// 	std::cout << (_lexer[i].range.startLine + 1) << "," << (_lexer[i].range.startColumn + 1) << "-";
		// 	std::cout << (_lexer[i].range.endLine + 1) << "," << (_lexer[i].range.endColumn + 1) << " ";
		// 	std::cout << TokenKindHelper::toStringView(_lexer[i].kind) << " " << _lexer[i].content << "\n";
		// }
		// return nullptr;

		_firstNode = true;

		while (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
			_advance(1);
		}

		// check for document annotation
		if (_get(0).kind == TokenKind::annotationKey) {
			_parseAnnotation();

			if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
				if (!_annotations.empty() && _firstNode) {
					_document->annotations = std::move(_annotations);
				}

				_advance(1);
			}
		}

		while (_isBound(0)) {
			Node* node = _parseAnnotatedBlock();
			if (node == nullptr) {
				if (_error || _stopped) {
					return _document;
				}

				_advance(1);
				continue;
			}

			_document->children.push_back(node);
		}

		if (_document && !_document->children.empty()) {
			_updateEndRange(_document->range, _document->children.back()->range);
		}

		return _document;
	}

	decltype(nullptr) _wrong(std::string_view message) {
		std::cout << "[ParsingError] " << message << "\n";
		return nullptr;
	}

	decltype(nullptr) _expect(std::string_view message) {
		if (!_isBound(0)) {
			std::cout << "[ParsingError] unxpected end of file, expect " << message << "\n";
			return nullptr;
		}

		std::string_view kind = TokenKindHelper::toStringView(_get(0).kind);
		std::cout << "[ParsingError] unxpected token " << kind << ", expect " << message << "\n";
		return nullptr;
	}

	bool _isBound(size_t offset) const {
		return _tokenIndex + offset < _lexer.size();
	}

	void _advance(size_t offset) {
		_tokenIndex += offset;
	}

	const Token& _get(size_t offset) const {
		return _lexer[_tokenIndex + offset];
	}

	const Token& _eat() {
		_tokenIndex += 1;
		return _lexer[_tokenIndex - 1];
	}

	Node* _parseEmphasis(Emphasis::Type type) {
		const Token& token = _eat();
		Emphasis* style = new Emphasis(token.range, type);

		while (_isBound(0) && _get(0).kind != token.kind) {
			Node* child = _parseInline();

			if (child == nullptr) {
				delete style;
				return nullptr;
			}

			style->children.push_back(child);
		}

		if (_isBound(0) && _get(0).kind != token.kind) {
			delete style;
			return _expect("asterisk");
		}

		_updateEndRange(style->range, _get(0).range);
		_advance(1);

		return style;
	}

	Node* _parseHighlight() {
		const Token& token = _eat();
		Highlight* highlight = new Highlight(token.range);

		while (_isBound(0) && _get(0).kind != token.kind) {
			Node* child = _parseInline();

			if (child == nullptr) {
				delete highlight;
				return nullptr;
			}

			highlight->children.push_back(child);
		}

		if (_isBound(0) && _get(0).kind != token.kind) {
			delete highlight;
			return _expect("equalequal");
		}

		_updateEndRange(highlight->range, _get(0).range);
		_advance(1);

		return highlight;
	}

	Node* _parseChange(Change::Type type, TokenKind closingKind) {
		Change* change = new Change(_eat().range, type);

		while (_isBound(0) && _get(0).kind != closingKind) {
			Node* child = _parseInline();

			if (child == nullptr) {
				delete change;
				return nullptr;
			}

			change->children.push_back(child);
		}

		if (_isBound(0) && _get(0).kind != closingKind) {
			delete change;
			return _expect("closing change");
		}

		_updateEndRange(change->range, _get(0).range);
		_advance(1);

		return change;
	}

	Node* _parseComment() {
		const Token& token = _eat();
		return new Comment(token.range, token.content);
	}

	Node* _parseText() {
		const Token& token = _eat();
		return new Text(token.range, token.content);
	}

	Node* _parseInline() {
		Node* node = nullptr;
		switch (_get(0).kind) {
			case TokenKind::comment: 
				node = _parseComment();
				break;

			case TokenKind::text: 
				node = _parseText();
				break;

			case TokenKind::asterisk: 
				node = _parseEmphasis(Emphasis::Type::bold);
				break;

			case TokenKind::slash: 
				node = _parseEmphasis(Emphasis::Type::italic);
				break;

			case TokenKind::underscore: 
				node = _parseEmphasis(Emphasis::Type::underline);
				break;

			case TokenKind::highlight: 
				node = _parseHighlight();
				break;

			case TokenKind::addOpen: 
				node = _parseChange(Change::Type::added, TokenKind::addClose);
				break;

			case TokenKind::removeOpen: 
				node = _parseChange(Change::Type::removed, TokenKind::removeClose);
				break;

			case TokenKind::lineBreak: 
				node = new LineBreak(_eat().range);
				break;

			case TokenKind::backtick: 
				node = _parseCode();
				break;

			case TokenKind::squareOpen: 
				node = _parseLink();
				break;

			case TokenKind::exclamation: 
				node = _parseView();
				break;

			case TokenKind::caret: {
				node = _parseFootnote();
				break;
			}

			case TokenKind::ampersand: {
				node = _parseInText();
				break;
			}

			case TokenKind::emoji: 
			       node = _parseEmoji();
			       break;

			case TokenKind::dateTime: 
			       node = _parseDateTime();
			       break;

			case TokenKind::hyphen: 
			       node = new Punct(_eat().range, Punct::Type::hypen);
			       break;

			case TokenKind::enDash: 
			       node = new Punct(_eat().range, Punct::Type::enDash);
			       break;

			case TokenKind::emDash: 
			       node = new Punct(_eat().range, Punct::Type::emDash);
			       break;

			case TokenKind::quoteOpen: 
			       node = new Punct(_eat().range, Punct::Type::quoteOpen);
			       break;

			case TokenKind::quoteClose: 
			       node = new Punct(_eat().range, Punct::Type::quoteClose);
			       break;

			case TokenKind::squoteOpen: 
			       node = new Punct(_eat().range, Punct::Type::squoteOpen);
			       break;

			case TokenKind::squoteClose: 
			       node = new Punct(_eat().range, Punct::Type::squoteClose);
			       break;

			case TokenKind::accountTag: 
			       node = new AccountTag(_get(0).range, _eat().content);
			       break;

			case TokenKind::hashTag: 
			       node = new HashTag(_get(0).range, _eat().content);
			       break;

			case TokenKind::colon: 
			       node = _parseText();
			       break;

			default:
				break;
		}

		return node;
	}

	Node* _parseParagraph() {
		const Token& token = _get(0);
		size_t previousTokenIndex = _tokenIndex;

		Paragraph* paragraph = new Paragraph(token.range);
		bool newline = false;

		Node* view = nullptr;
		size_t viewIndex = 0;
		size_t commentCounts = 0;
		size_t viewCounts = 0;
		size_t otherCounts = 0;

		while (_isBound(0) && _isParagraph()) {
			Node* node = _parseInline();

			if (node == nullptr) {
				if (_get(0).kind == TokenKind::coloncolon) {
					if (!newline) {
						delete paragraph;
						_tokenIndex = previousTokenIndex;
						return _parseDefinitionList();
					} else {
						_advance(1);
					}
				}

				if (_get(0).kind == TokenKind::newline) {
					newline = true;

					if (_isBound(1) && _get(1).kind == TokenKind::indentOpen) {
						_advance(1);

						Node* indent = _parseIndent();
						if (indent == nullptr) {
							delete paragraph;
							return nullptr;
						}

						paragraph->children.push_back(indent);
						continue;
					}

					if (_isBound(1) && _get(1).kind == TokenKind::indentClose) {
						_advance(1);
						continue;
					}

					const Token& token = _eat();
					paragraph->children.push_back(new Space(token.range));
					continue;
				}

				continue;
			}

			switch (node->kind) {
				case NodeKind::comment:
					commentCounts += 1;
					break;
				case NodeKind::view:
					viewCounts += 1;
					viewIndex = paragraph->children.size();
					view = node;
					break;
				default:
					otherCounts += 1;
					break;
			}

			paragraph->children.push_back(node);
		}

		if (_get(0).kind == TokenKind::newlinePlus) {
			_advance(1);
		}

		if (otherCounts == 0) {
			// paragraph with only single view as a child 
			if (viewCounts == 1) {
				Node* blockView = view;
				blockView->children = std::move(paragraph->children);
				blockView->children.erase(blockView->children.begin() + viewIndex);
				delete paragraph;
				return blockView;
			} else { // comments only
				if (_annotations.empty()) {
					// delete paragraph;
					// return nullptr;
				}
			}
		}

		if (paragraph && !paragraph->children.empty()) {
			_updateEndRange(paragraph->range, paragraph->children.back()->range);
		}

		return paragraph;
	}

	int getHeadValue(TokenKind token) {
		switch (token) {
			case TokenKind::head3: return 1;
			case TokenKind::head2: return 2;
			case TokenKind::head1: return 3;
			default: return 0;
		}
	}

	Node* _parseHeading() {
		const Token& token = _get(0);
		Heading* heading = new Heading(token.range);

		switch (token.kind) {
			case TokenKind::head3:
				heading->type = Heading::Type::chapter;
				break;

			case TokenKind::head2:
				heading->type = Heading::Type::section;
				break;

			case TokenKind::head1:
				heading->type = Heading::Type::subsection;
				break;

			default: 
				delete heading;
				return nullptr;
		}

		Node* title = _parseTitle();
		if (title == nullptr) {
			delete heading;
			return nullptr;
		}

		heading->children.push_back(title);

		int currentValue = getHeadValue(token.kind);

		while (_isBound(0)) {
			switch (_get(0).kind) {
				case TokenKind::head1:
				case TokenKind::head2:
				case TokenKind::head3:
					if (!(getHeadValue(_get(0).kind) > currentValue)) {
						goto end;
					}
					break;
				default: break;
			}

			Node* node = _parseAnnotatedBlock();

			if (node == nullptr) {
				delete heading;
				return nullptr;
			}

			heading->children.push_back(node);
		}

		end:

		if (heading && !heading->children.empty()) {
			_updateEndRange(heading->range, heading->children.back()->range);
		}

		return heading;
	}

	Node* _parseTitle() {
		const Token& token = _eat();
		Title* title = new Title(token.range);

		while (_isBound(0)) {
			Node* node = _parseInline();
			if (node == nullptr) {
				if (_get(0).kind == TokenKind::newlinePlus) {
					_advance(1);
					break;
				}

				if (_get(0).kind == TokenKind::newline) {
					_advance(1);

					if (_isBound(0) && _get(0).kind == TokenKind::head1)  {
						Node* subtitle = _parseSubtitle();
						if (subtitle == nullptr) {
							delete title;
							return nullptr;
						}

						title->children.push_back(subtitle);
					}

					break;
				}

				delete title;
				return _expect("newline or block");
			}

			title->children.push_back(node);
		}

		if (title && !title->children.empty()) {
			_updateEndRange(title->range, title->children.back()->range);
		}

		return title;
	}

	Node* _parseSubtitle() {
		const Token& token = _eat();
		Subtitle* subtitle = new Subtitle(token.range);

		while (_isBound(0)) {
			Node* node = _parseInline();
			if (node == nullptr) {
				if (_get(0).kind == TokenKind::newlinePlus) {
					_advance(1);
					break;
				}

				if (_get(0).kind == TokenKind::newline) {
					_advance(1);
					break;
				}

				delete subtitle;
				return _expect("newline or block");
			}

			subtitle->children.push_back(node);
		}

		if (subtitle && !subtitle->children.empty()) {
			_updateEndRange(subtitle->range, subtitle->children.back()->range);
		}

		return subtitle;
	}

	Node* _parseIndent() {
		const Token& token = _eat();
		Quote* indent = new Quote(token.range);

		while (_isBound(0) && _get(0).kind != TokenKind::indentClose) {
			Node* node = _parseAnnotatedBlock();

			if (node == nullptr) {
				delete indent;
				return nullptr;
			}

			indent->children.push_back(node);
		}

		if (!(_isBound(0) && _get(0).kind == TokenKind::indentClose)) {
			return _expect("indent pop");
		}

		_advance(1);

		if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
			_eat();
		}

		if (indent && !indent->children.empty()) {
			_updateEndRange(indent->range, indent->children.back()->range);
		}

		return indent;
	}

	Node* _parsePageBreak() {
		Node* node = new PageBreak(_eat().range);

		if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
			_advance(1);
		}

		return node;
	}

	Node* _parseDinkus() {
		Node* node = new Dinkus(_eat().range);

		if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
			_advance(1);
		}

		return node;
	}

	enum class ItemResult {
		ok,
		error,
		earlyExit,
	};

	ItemResult _parseItem(Node* list, Node* item) {
		while (_isBound(0)) {
			Node* node = _parseInline();

			if (node == nullptr) {
				if (_get(0).kind == TokenKind::newline) {
					if (_isBound(1) && _get(1).kind == TokenKind::indentOpen) {
						_advance(2);

						while (_isBound(0)) {
							Node* subnode = _parseAnnotatedBlock();
							if (subnode == nullptr) {
								if (_get(0).kind == TokenKind::indentClose) {
									_advance(1);
									break;
								}

								_expect("indent pop");
								return ItemResult::error;
							}

							item->children.push_back(subnode);
						}

						break;
					}

					_advance(1);
					break;
				}
				
				if (_get(0).kind == TokenKind::newlinePlus) {
					_advance(1);
					return ItemResult::earlyExit;
				}

				return ItemResult::earlyExit;
			}

			item->children.push_back(node);
		}

		return ItemResult::ok;
	}

	Node* _parseList(TokenKind tokenKind, NodeKind nodeKind) {
		List* list = new List(_get(0).range, nodeKind);

		while (_isBound(0) && _get(0).kind == tokenKind) {
			Item* item = new Item(_eat().range);
			list->children.push_back(item);

			ItemResult result = _parseItem(list, item);

			if (item && !item->children.empty()) {
				_updateEndRange(item->range, item->children.back()->range);
			}

			switch (result) {
				case ItemResult::ok: break;
				case ItemResult::error: 
					delete list;
					return nullptr;
				case ItemResult::earlyExit: goto listEnd;
			}
		}

		listEnd:

		if (list && !list->children.empty()) {
			_updateEndRange(list->range, list->children.back()->range);
		}

		return list;
	}

	Node* _parseCheckList() {
		List* list = new List(_get(0).range, NodeKind::checkList);

		while (_isBound(0) && _get(0).kind == TokenKind::checkbox) {
			const Token& token = _eat();
			CheckItem* item = new CheckItem(token.range);
			list->children.push_back(item);

			switch (token.content[1]) {
				case ' ': item->checked = false; break;
				case 'x': item->checked = true; break;
			}

			ItemResult result = _parseItem(list, item);

			if (item && !item->children.empty()) {
				_updateEndRange(item->range, item->children.back()->range);
			}

			switch (result) {
				case ItemResult::ok: break;
				case ItemResult::error:
					delete list;
					return nullptr;
				case ItemResult::earlyExit: goto listEnd;
			}
		}

		listEnd:

		if (list && !list->children.empty()) {
			_updateEndRange(list->range, list->children.back()->range);
		}

		return list;
	}

	Node* _parseDefinitionList() {
		List* list = new List(_get(0).range, NodeKind::definitionList);

		while (_isBound(0) && _isParagraph()) {
			size_t previousTokenIndex = _tokenIndex;
			bool itemOccupied = false;
			bool itemColoncolon = false;

			DefinitionItem* item = new DefinitionItem(_get(0).range);
			DefinitionTerm* term = new DefinitionTerm(_get(0).range);

			item->children.push_back(term);

			while (_isBound(0) && _isParagraph()) {
				Node* node = _parseInline();

				if (node == nullptr) {
					if (_get(0).kind == TokenKind::newlinePlus) {
						_advance(1);
						if (!itemOccupied) {
							delete item;
						}
						goto listEnd;
					}

					if (_get(0).kind == TokenKind::coloncolon) {
						DefinitionDesc* desc = new DefinitionDesc(_eat().range);
						item->children.push_back(desc);
						itemColoncolon = true;

						while (_isBound(0)) {
							Node* node = _parseInline();
							if (node == nullptr) {
								if (_get(0).kind == TokenKind::newline) {
									if (_isBound(1) && _get(1).kind == TokenKind::indentOpen) {
										_advance(2);

										while (_isBound(0)) {
											Node* subnode = _parseAnnotatedBlock();
											if (subnode == nullptr) {
												if (_get(0).kind == TokenKind::indentClose) {
													_advance(1);
													goto itemEnd;
												}

												_expect("indent pop");
												return list;
											}

											desc->children.push_back(subnode);
										}

										goto itemEnd;
									}

									_advance(1);
									goto itemEnd;
								}

								if (_get(0).kind == TokenKind::newlinePlus) {
									_advance(1);
									list->children.push_back(item);
									goto listEnd;
								}
							}
							desc->children.push_back(node);
						}
					}
					break;
				}

				term->children.push_back(node);
				itemOccupied = true;
			}

			itemEnd:

			if (itemColoncolon) {
				list->children.push_back(item);

				if (!item->children.empty()) {
					_updateEndRange(item->children.front()->range, item->children.front()->children.back()->range);
					_updateEndRange(item->children.back()->range, item->children.back()->children.back()->range);

					_updateEndRange(item->range, item->children.back()->range);
				}


				goto listEnd;
			}
			continue;
		}

		listEnd:

		if (list && !list->children.empty()) {
			_updateEndRange(list->range, list->children.back()->range);
		}

		return list;
	}

	Node* _checkTableRow(Node* node, Row::Type type) {
		// content only table
		if (type == Row::Type::header) {
			for (size_t i = 0; i < node->children.size(); i += 1) {
				static_cast<Row*>(node->children[i])->type = Row::Type::content;
			}
		}

		return node;
	}

	Node* _parseTable() {
		Table* table = new Table(_get(0).range);

		Row::Type type = Row::Type::header;

		while (_isBound(0) && _get(0).kind == TokenKind::pipe) {
			const Token& token = _eat();

			if (_isBound(0) && (_get(0).kind == TokenKind::tee || _get(0).kind == TokenKind::teeLeft || _get(0).kind == TokenKind::teeCenter || _get(0).kind == TokenKind::teeRight)) {
				while (_isBound(0)) {
					while (_isBound(0)) {
						switch (_get(0).kind) {
							case TokenKind::tee:
								_advance(1);
								goto nextTeeCell;

							case TokenKind::teeLeft:
								_advance(1);
								if (type == Row::Type::header) {
									table->alignments.push_back(Table::Alignment::left);
								}
								goto nextTeeCell;

							case TokenKind::teeCenter:
								_advance(1);
								if (type == Row::Type::header) {
									table->alignments.push_back(Table::Alignment::center);
								}
								goto nextTeeCell;

							case TokenKind::teeRight:
								_advance(1);
								if (type == Row::Type::header) {
									table->alignments.push_back(Table::Alignment::right);
								}
								goto nextTeeCell;

							case TokenKind::pipe:
								_advance(1);
								goto nextTeeCell;

							case TokenKind::newline:
								_advance(1);
								goto nextTeeRow;

							case TokenKind::newlinePlus:
								_advance(1);
								goto returnWithoutCheck;
								
							default:
								goto returnWithoutCheck;
						}
					}

					nextTeeCell:
					continue;
				}

				nextTeeRow:
				if (type == Row::Type::header) {
					type = Row::Type::content;
				} else if (type == Row::Type::content) {
					type = Row::Type::footer;
				}
				continue;
			}

			Row* row = new Row(token.range);
			row->type = type;
			table->children.push_back(row);

			while (_isBound(0)) {
				Cell* cell = new Cell(_get(0).range);

				while (_isBound(0)) {
					Node* node = _parseInline();
					if (node == nullptr) {
						switch (_get(0).kind) {
							case TokenKind::pipe:
								_advance(1);
								goto nextCell;

							case TokenKind::newline:
								_advance(1);
								delete cell;
								goto nextRow;

							case TokenKind::newlinePlus:
								_advance(1);
								delete cell;
								if (row && !row->children.empty()) {
									_updateEndRange(row->range, row->children.back()->range);
								}
								goto returnWithCheck;
								
							default:
								delete cell;
								if (row && !row->children.empty()) {
									_updateEndRange(row->range, row->children.back()->range);
								}
								goto returnWithCheck;
						}
					}

					cell->children.push_back(node);
				}

				nextCell:
				row->children.push_back(cell);

				if (cell && !cell->children.empty()) {
					_updateEndRange(cell->range, cell->children.back()->range);
					cell->range.endColumn += 1; // account for pipe
				}
			}

			nextRow:
			if (row && !row->children.empty()) {
				_updateEndRange(row->range, row->children.back()->range);
			}

			continue;
		}

		returnWithCheck:
		_checkTableRow(table, type);

		returnWithoutCheck:
		if (table && !table->children.empty()) {
			_updateEndRange(table->range, table->children.back()->range);
		}

		return table;
	}


	Node* _parseLink() {
		Link* link = new Link(_eat().range);
		Range rangeEnd;

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			link->setResource(_eat().content);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::squareClose) {
			rangeEnd = _get(0).range;
			_advance(1);

			if (_isBound(0) && _get(0).kind == TokenKind::parenOpen) {
				if (_isBound(2) && 
					_get(1).kind == TokenKind::raw && 
					_get(2).kind == TokenKind::parenClose) {

					link->label = _get(1).content;

					rangeEnd = _get(2).range;
					_advance(3);
				} else {
					delete link;
					return nullptr;
				}
			}
		}

		_updateEndRange(link->range, rangeEnd);

		return link;
	}

	Node* _parseView() {
		View* view = new View(_eat().range);
		Range rangeEnd;

		if (_isBound(0) && _get(0).kind == TokenKind::squareOpen) {
			_advance(1);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			view->resource = _eat().content;
		}

		if (_isBound(0) && _get(0).kind == TokenKind::squareClose) {
			rangeEnd = _get(0).range;
			_advance(1);

			if (_isBound(0) && _get(0).kind == TokenKind::parenOpen) {
				if (_isBound(2) && 
					_get(1).kind == TokenKind::raw && 
					_get(2).kind == TokenKind::parenClose) {

					view->label = _get(1).content;

					rangeEnd = _get(2).range;
					_advance(3);
				} else {
					delete view;
					return nullptr;
				}
			}
		}

		_updateEndRange(view->range, rangeEnd);

		return view;
	}

	Node* _parseInText() {
		InText* view = new InText(_eat().range);

		if (_isBound(0) && _get(0).kind == TokenKind::squareOpen) {
			_advance(1);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			view->id = _eat().content;
		}

		if (_isBound(0) && _get(0).kind == TokenKind::squareClose) {
			_updateEndRange(view->range, _get(0).range);
			_advance(1);
		}

		return view;
	}

	Node* _parseEmoji() {
		const Token& token = _eat();
		return new Emoji(token.range, token.content);
	}

	Node* _parseDateTime() {
		const Token& token = _eat();
		return new DateTime(token.range, token.content);
	}

	Node* _parseInclude() {
		#ifdef __EMSCRIPTEN__
		Link* link = new Link(_eat().range);

		if (_isBound(0) && _get(0).kind == TokenKind::squareOpen) {
			_advance(1);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			link->resource = _eat().content;
		}

		if (_isBound(0) && _get(0).kind == TokenKind::squareClose) {
			_updateEndRange(link->range, _get(0).range);
			_advance(1);
		}

		if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
			_advance(1);
		}

		return link;
		#else

		Document* document = nullptr;
		const Token& token = _eat();

		if (_isBound(0) && _get(0).kind == TokenKind::squareOpen) {
			_advance(1);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			std::string_view filePath = _eat().content;
			std::string path = _workspaceFolder + std::string("/");
			path.append(filePath);

			if (_fileInclusion) {
				Parser parser;
				if (std::filesystem::exists(path)) {
					if (std::filesystem::is_directory(path)) {
						std::cout << "inclusion failed because \"" << path << "\" is a folder\n";
						_error = true;
						return nullptr;
					}

					document = parser.parseFile(path);

					if (document == nullptr) {
						return nullptr;
					}

					document->range = token.range;
					parser._document = nullptr;
				} else {
					std::cout << "inclusion failed because file \"" << path << "\" does not exists\n";
					_error = true;
					return nullptr;
				}
			} else {
				document = new Document();
				document->path = std::string(filePath.data(), filePath.size());
				document->range = token.range;
			}
		}

		if (_isBound(0) && _get(0).kind == TokenKind::squareClose) {
			_updateEndRange(document->range, _get(0).range);
			_advance(1);
		}

		if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
			_advance(1);
		}

		return document;
		#endif
	}

	Node* _parseFootnote() {
		const Token& token = _eat();
		Footnote* ref = nullptr;

		if (_isBound(0) && _get(0).kind == TokenKind::squareOpen) {
			_advance(1);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			ref = new Footnote(token.range, _eat().content);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::squareClose) {
			_updateEndRange(ref->range, _get(0).range);
			_advance(1);
		}

		return ref;
	}

	Node* _parseReference() {
		Reference* ref = new Reference(_get(0).range);
		ref->id = _get(0).content;

		_advance(1);
		_parseAnnotation();

		ref->infos = std::move(_annotations);

		return ref;
	}

	Node* _parseCode() {
		Code* code = new Code(_eat().range);
		Range endRange;

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			code->content = _eat().content;
		}

		if (_isBound(0) && _get(0).kind == TokenKind::backtick) {
			endRange = _get(0).range;
			_advance(1);

			if (_isBound(2) && 
				_get(0).kind == TokenKind::backtick && 
				_get(1).kind == TokenKind::raw && 
				_get(2).kind == TokenKind::backtick) {

				code->label = code->content;
				code->content = _get(1).content;

				endRange = _get(2).range;
				_advance(3);
			}
		}

		_updateEndRange(code->range, endRange);

		return code;
	}

	Node* _parseCodeBlock() {
		CodeBlock* codeBlock = new CodeBlock(_eat().range);

		if (_isBound(0) && _get(0).kind == TokenKind::text) {
			codeBlock->label = _eat().content;
		}

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			codeBlock->content = _eat().content;
		}

		if (_isBound(0) && _get(0).kind == TokenKind::fenceClose) {
			_updateEndRange(codeBlock->range, _get(0).range);
			_advance(1);

			if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
				_advance(1);
			}
		}

		return codeBlock;
	}

	Node* _parseAdmon() {
		const Token& token = _eat();
		Admon* admon = new Admon(token.range, token.content);

		while (_isBound(0) && _isParagraph()) {
			Node* node = _parseInline();

			if (node == nullptr) {
				if (_get(0).kind == TokenKind::newline) {
					if (_isBound(1) && _get(1).kind == TokenKind::indentOpen) {
						admon->children.push_back(new Space(_get(0).range));
						_advance(2);

						while (_isBound(0)) {
							Node* subnode = _parseAnnotatedBlock();
							if (subnode == nullptr) {
								if (_get(0).kind == TokenKind::indentClose) {
									_advance(1);
									if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
										_advance(1);
									}
									goto end;
								}

								delete admon;
								return _expect("indent pop");
							}

							admon->children.push_back(subnode);
						}

						break;
					}

					_advance(1);
					break;
				}
				
				if (_get(0).kind == TokenKind::newlinePlus) {
					_advance(1);
					goto end;
				}
			}

			admon->children.push_back(node);
		}

		end:

		if (admon && !admon->children.empty()) {
			_updateEndRange(admon->range, admon->children.back()->range);
		}

		return admon;
	}

	void _parseAnnotation() {
		while (_isBound(0) && _get(0).kind == TokenKind::annotationKey) {
			Pair annotation;
			annotation.key = _eat().content;
			if (_isBound(0) && _get(0).kind == TokenKind::annotationValue) {
				annotation.value = _eat().content;
			}
			_annotations.push_back(annotation);
		}
	}

	bool _isParagraph() {
		switch (_get(0).kind) {
			case TokenKind::comment:
			case TokenKind::text:
			case TokenKind::newline:

			case TokenKind::asterisk:
			case TokenKind::slash:
			case TokenKind::underscore:
			case TokenKind::backtick:
			case TokenKind::highlight:
			case TokenKind::addOpen:
			case TokenKind::removeOpen:

			case TokenKind::squareOpen:
			case TokenKind::exclamation:
			case TokenKind::caret:
			case TokenKind::ampersand:
			case TokenKind::emoji:
			case TokenKind::dateTime:

			case TokenKind::hyphen:
			case TokenKind::enDash:
			case TokenKind::emDash:

			case TokenKind::quoteOpen:
			case TokenKind::quoteClose:
			case TokenKind::squoteOpen:
			case TokenKind::squoteClose:

			case TokenKind::lineBreak:

			case TokenKind::accountTag:
			case TokenKind::hashTag:

			case TokenKind::colon:
			case TokenKind::coloncolon:
				return true;

			default: 
				return false;
		}
	}

	Node* _parseAnnotatedBlock() {
		while (_get(0).kind == TokenKind::annotationKey) {
			_parseAnnotation();

			if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
				_advance(1);
			}
		}

		Node* node = _parseBlock();

		if (node != nullptr) {
			_firstNode = false;
			if (_annotations.size() != 0) {
				node->annotations = std::move(_annotations);
			}
		}

		if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
			_advance(1);
		}

		return node;
	}

	Node* _parseBlock() {
		if (!_isBound(0)) {
			return nullptr;
		}

		Node* node = nullptr;

		switch (_get(0).kind) {
			case TokenKind::comment:
			case TokenKind::text:

			case TokenKind::asterisk:
			case TokenKind::underscore:
			case TokenKind::backtick:
			case TokenKind::highlight:
			case TokenKind::addOpen:
			case TokenKind::removeOpen:

			case TokenKind::squareOpen:
			case TokenKind::exclamation:
			case TokenKind::emoji:
			case TokenKind::dateTime:

			case TokenKind::hyphen:
			case TokenKind::enDash:
			case TokenKind::emDash:

			case TokenKind::quoteOpen:
			case TokenKind::quoteClose:
			case TokenKind::squoteOpen:
			case TokenKind::squoteClose:

			case TokenKind::lineBreak:

			case TokenKind::accountTag:
			case TokenKind::hashTag:
			case TokenKind::caret:
			case TokenKind::ampersand:

			case TokenKind::colon:
				node = _parseParagraph();
				break;

			case TokenKind::referenceKey:
				node = _parseReference();
				break;

			case TokenKind::head1:
			case TokenKind::head2:
			case TokenKind::head3:
				node = _parseHeading();
				break;

			case TokenKind::indentOpen:
				node = _parseIndent();
				break;

			case TokenKind::pageBreak:
				node = _parsePageBreak();
				break;

			case TokenKind::documentBreak:
				_stopped = true;
				return nullptr;

			case TokenKind::dinkus:
				node = _parseDinkus();
				break;

			case TokenKind::question:
				node = _parseInclude();
				break;

			case TokenKind::bullet:
				node = _parseList(TokenKind::bullet, NodeKind::list);
				break;

			case TokenKind::index:
				node = _parseList(TokenKind::index, NodeKind::numberedList);
				break;

			case TokenKind::checkbox:
				node = _parseCheckList();
				break;

			case TokenKind::pipe:
				node = _parseTable();
				break;

			case TokenKind::fenceOpen:
				node = _parseCodeBlock();
				break;

			case TokenKind::admon:
				node = _parseAdmon();
				break;

			default:
				break;
		}

		return node;
	}

	void _updateEndRange(Range& start, const Range& end) {
		start.endLine = end.endLine;
		start.endColumn = end.endColumn;
	}

private:
	Lexer _lexer;

	size_t _tokenIndex;

	Document* _document;

	std::string _workspaceFolder;

	bool _fileInclusion;

	bool _error;

	bool _stopped;

	bool _firstNode;

	std::vector<Pair> _annotations;
};

}
