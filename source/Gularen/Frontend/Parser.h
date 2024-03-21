#pragma once

#include "Gularen/Frontend/Node.h"
#include <fstream>

// TODO: allow double newline on blockquote 
// TODO: escaping with \

namespace Gularen {

class Parser {
public:
	Parser() {
		_document = nullptr;
		_fileInclusion = true;
	}

	~Parser() {
		delete _document;
		_document = nullptr;
	}

	Document* parseFile(std::string_view path) {
		_document = new Document();
		_document->path = path;

		for (size_t i = path.size(); i > 0; i -= 1) {
			if (path[i - 1] == '/') {
				_documentPath = std::string(path.data(), i - 1);
				break;
			}
		}

		if (_documentPath.size() == 0) {
			_documentPath = ".";
		}

		std::ifstream file(path);

		if (!file.is_open()) {
			delete _document;
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

	void setFileInclusion(bool state) {
		_fileInclusion = state;
	}

private:
	Document* _parse(std::string_view content) {
		_lexer.parse(content);
		_tokenIndex = 0;

		for (size_t i = 0; i < _lexer.size(); i += 1) {
			std::cout << (_lexer[i].range.startLine + 1) << "," << (_lexer[i].range.startColumn + 1) << "-";
			std::cout << (_lexer[i].range.endLine + 1) << "," << (_lexer[i].range.endColumn + 1) << " ";
			_lexer[i].print();
		}
		// return nullptr;

		bool firstAnnotation = true;

		while (_isBound(0)) {
			if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
				if (!_annotations.empty()) {
					if (firstAnnotation) {
						firstAnnotation = false;
						_document->annotations = std::move(_annotations);
					} else {
						Node* node = new Paragraph(_get(0).range);
						node->annotations = std::move(_annotations);
						_document->children.push_back(node);
					}
				}

				_advance(1);
			}

			if (_get(0).kind == TokenKind::annotationKey) {
				_parseAnnotation();
				continue;
			}

			Node* node = _parseBlock();
			if (node == nullptr) {
				_advance(1);
				continue;
			}

			_document->children.push_back(node);

			if (_annotations.size() != 0) {
				node->annotations = std::move(_annotations);
			}
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

		std::string_view kind = toStringView(_get(0).kind);
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

	Node* _parseStyle(Style::Type type) {
		const Token& token = _eat();
		Style* style = new Style(token.range, type);

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
			return _expect("equal");
		}

		_updateEndRange(highlight->range, _get(0).range);
		_advance(1);

		return highlight;
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
				node = _parseStyle(Style::Type::bold);
				break;

			case TokenKind::underscore: 
				node = _parseStyle(Style::Type::italic);
				break;

			case TokenKind::equal: 
				node = _parseHighlight();
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
				if (_isBound(1)) {
					if (_get(1).kind == TokenKind::parenOpen) {
						node = _parseFootnote();
						break;
					}
					if (_get(1).kind == TokenKind::squareOpen) {
						node = _parseInText();
						break;
					}
				}
				node = nullptr;
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

	Node* _parseHeading() {
		const Token& token = _eat();
		Heading* heading = new Heading(token.range);

		switch (token.kind) {
			case TokenKind::head3:
				heading->type = Heading::Type::section;
				break;

			case TokenKind::head2:
				heading->type = Heading::Type::subsection;
				break;

			case TokenKind::head1:
				heading->type = Heading::Type::subsubsection;
				break;

			default: 
				delete heading;
				return nullptr;
		}

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
							delete heading;
							return nullptr;
						}

						heading->children.push_back(subtitle);
					}

					break;
				}

				delete heading;
				return _expect("newline or block");
			}

			heading->children.push_back(node);
		}

		if (heading && !heading->children.empty()) {
			_updateEndRange(heading->range, heading->children.back()->range);
		}

		return heading;
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
		Indent* indent = new Indent(token.range);

		while (_isBound(0) && _get(0).kind != TokenKind::indentClose) {
			Node* node = _parseBlock();

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
							Node* subnode = _parseBlock();
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
											Node* subnode = _parseBlock();
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
		Link* link = new Link(_eat().position);

		if (_isBound(0) && _get(0).kind == TokenKind::squareOpen) {
			_advance(1);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			link->resource = _eat().content;
		}

		if (_isBound(0) && _get(0).kind == TokenKind::squareClose) {
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
			std::string path = _documentPath + std::string("/");
			path.append(filePath);

			if (_fileInclusion) {
				Parser parser;
				document = parser.parseFile(std::string_view(path.data(), path.size()));

				if (document != nullptr) {
					document->range = token.range;
				}

				parser._document = nullptr;
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

		if (_isBound(0) && _get(0).kind == TokenKind::parenOpen) {
			_advance(1);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			ref = new Footnote(token.range, _eat().content);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::parenClose) {
			_updateEndRange(ref->range, _get(0).range);
			_advance(1);
		}

		return ref;
	}

	Node* _parseReference() {
		Reference* ref = new Reference(_get(0).range);
		ref->id = _get(2).content;

		_advance(6);

		if (_isBound(0) && _get(0).kind == TokenKind::indentOpen) {
			_advance(1);

			while (_isBound(0)) {
				// std::cout << "GotToken %.*s\n", 
				// 	tostd::string_view(_get(0).kind).size(),
				// 	tostd::string_view(_get(0).kind).data()
				// );
				//
				if (_get(0).kind == TokenKind::indentClose) {
					_advance(1);
					break;
				}

				if (!(_isBound(1) && _get(0).kind == TokenKind::text && _get(1).kind == TokenKind::colon)) {
					break;
				}
				ReferenceInfo* info = new ReferenceInfo(_get(0).range, _get(0).content);
				_advance(2);

				while (_isBound(0) && (_get(0).kind != TokenKind::newline || _get(0).kind != TokenKind::newlinePlus)) {
					Node* node = _parseInline();
					if (node == nullptr) {
						break;
					}
					info->children.push_back(node);
				}

				if (info && !info->children.empty()) {
					_updateEndRange(info->range, info->children.back()->range);
				}

				ref->children.push_back(info);

				if (_isBound(0)) {
					if (_get(0).kind == TokenKind::newline) {
						_advance(1);
					}

					if (_get(0).kind == TokenKind::newlinePlus) {
						_advance(1);
						break;
					}
				}
			}
		}

		if (ref && !ref->children.empty()) {
			_updateEndRange(ref->range, ref->children.back()->range);
		}

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

	Node* _parseBlockquote() {
		const Token& token = _eat();
		Blockquote* indent = new Blockquote(token.range);

		while (_isBound(0) && _get(0).kind != TokenKind::blockquoteClose) {
			Node* node = _parseBlock();

			if (node == nullptr) {
				delete indent;
				return nullptr;
			}

			indent->children.push_back(node);
		}

		if (!(_isBound(0) && _get(0).kind == TokenKind::blockquoteClose)) {
			return _expect("blockquote pop");
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
							Node* subnode = _parseBlock();
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
			Annotation annotation;
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
			case TokenKind::underscore:
			case TokenKind::backtick:
			case TokenKind::equal:

			case TokenKind::squareOpen:
			case TokenKind::exclamation:
			case TokenKind::caret:
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
			case TokenKind::equal:

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

			case TokenKind::colon:
				node = _parseParagraph();
				break;

			case TokenKind::caret: {
				if (_isBound(5) && 
					_get(1).kind == TokenKind::squareOpen &&
					_get(2).kind == TokenKind::raw &&
					_get(3).kind == TokenKind::squareClose &&
					_get(4).kind == TokenKind::colon &&
					_get(5).kind == TokenKind::newline
					) {
					node = _parseReference();
					break;
				}

				node = _parseParagraph();
				break;
			}

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

			case TokenKind::blockquoteOpen:
				node = _parseBlockquote();
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

	std::string _documentPath;

	bool _fileInclusion;

	std::vector<Annotation> _annotations;
};

}
