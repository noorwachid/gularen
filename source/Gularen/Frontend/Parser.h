#pragma once

#include "Gularen/Frontend/Node.h"
#include "Gularen/Library/String.h"
#include <stdio.h>

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

	Document* parseFile(StringSlice path) {
		_document = new Document();
		_document->path = String(path.pointer(), path.size());

		for (unsigned int i = path.size(); i > 0; i -= 1) {
			if (path.get(i - 1) == '/') {
				_documentPath = String(path.pointer(), i - 1);
				break;
			}
		}

		if (_documentPath.size() == 0) {
			_documentPath = String(".");
		}

		char pathC[512];
		unsigned int pathSize = path.size() < 511 ? path.size() : 511;
		memcpy(pathC, path.pointer(), pathSize);
		pathC[pathSize] = 0;

		FILE* file = fopen(pathC, "r");

		if (file == nullptr) {
			delete _document;
			_document = nullptr;
			return nullptr;
		}

		fseek(file, 0, SEEK_END);
		char* data = _document->content.expand(ftell(file));
		fseek(file, 0, SEEK_SET);

		fread(data, _document->content.size(), sizeof(char), file);
		fclose(file);

		return _parse(StringSlice(_document->content.pointer(), _document->content.size()));
	}

	Document* parse(StringSlice content) {
		_document = new Document();

		return _parse(content);
	}

	void setFileInclusion(bool state) {
		_fileInclusion = state;
	}

private:
	Document* _parse(StringSlice content) {
		Lexer lexer;
		_tokens = lexer.parse(content);
		_tokenIndex = 0;

		// for (unsigned int i = 0; i < _tokens.size(); i += 1) {
		// 	_tokens.get(i).print();
		// }
		// return nullptr;

		bool firstAnnotation = true;

		while (_isBound(0)) {
			if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
				_advance(1);

				if (firstAnnotation) {
					firstAnnotation = false;
					_document->annotations = _annotations;
					_annotations = Array<Annotation>();
				}
			}

			if (_get(0).kind == TokenKind::annotationKey) {
				_parseAnnotation();
				continue;
			}

			Node* node = _parseBlock();
			if (node == nullptr) {
				continue;
			}
			_document->children.append(node);

			if (_annotations.size() != 0) {
				node->annotations = _annotations;
				_annotations = Array<Annotation>();
			}
		}

		return _document;
	}

	decltype(nullptr) _wrong(StringSlice message) {
		printf("[ParsingError] %.*s\n", message.size(), message.pointer());
		return nullptr;
	}

	decltype(nullptr) _expect(StringSlice message) {
		if (!_isBound(0)) {
			printf("[ParsingError] unxpected end of file, expect %.*s\n", message.size(), message.pointer());
			return nullptr;
		}

		StringSlice kind = toStringSlice(_get(0).kind);
		printf("[ParsingError] unxpected token %.*s, expect %.*s\n", kind.size(), kind.pointer(), message.size(), message.pointer());
		return nullptr;
	}

	bool _isBound(unsigned int offset) const {
		return _tokenIndex + offset < _tokens.size();
	}

	void _advance(unsigned int offset) {
		_tokenIndex += offset;
	}

	const Token& _get(unsigned int offset) const {
		return _tokens.get(_tokenIndex + offset);
	}

	const Token& _eat() {
		_tokenIndex += 1;
		return _tokens.get(_tokenIndex - 1);
	}

	Node* _parseStyle(Style::Type type) {
		const Token& token = _eat();
		Style* style = new Style(token.position, type);

		while (_isBound(0) && _get(0).kind != token.kind) {
			Node* child = _parseInline();

			if (child == nullptr) {
				delete style;
				return nullptr;
			}

			style->children.append(child);
		}

		if (_isBound(0) && _get(0).kind != token.kind) {
			delete style;
			return _expect("asterisk");
		}

		_advance(1);

		return style;
	}

	Node* _parseHighlight() {
		const Token& token = _eat();
		Highlight* highlight = new Highlight(token.position);

		while (_isBound(0) && _get(0).kind != token.kind) {
			Node* child = _parseInline();

			if (child == nullptr) {
				delete highlight;
				return nullptr;
			}

			highlight->children.append(child);
		}

		if (_isBound(0) && _get(0).kind != token.kind) {
			delete highlight;
			return _expect("equal");
		}

		_advance(1);

		return highlight;
	}

	Node* _parseComment() {
		const Token& token = _eat();
		return new Comment(token.position, token.content);
	}

	Node* _parseText() {
		const Token& token = _eat();
		return new Text(token.position, token.content);
	}

	Node* _parseInline() {
		switch (_get(0).kind) {
			case TokenKind::comment: return _parseComment();

			case TokenKind::text: return _parseText();

			case TokenKind::asterisk: return _parseStyle(Style::Type::bold);
			case TokenKind::underscore: return _parseStyle(Style::Type::italic);
			case TokenKind::equal: return _parseHighlight();

			case TokenKind::lineBreak: return new LineBreak(_eat().position);

			case TokenKind::backtick: return _parseCode();
			case TokenKind::squareOpen: return _parseLink();
			case TokenKind::exclamation: return _parseView();
			case TokenKind::caret: return _parseFootnote();
			case TokenKind::emoji: return _parseEmoji();
			case TokenKind::dateTime: return _parseDateTime();

			case TokenKind::hyphen: return new Punct(_eat().position, Punct::Type::hypen);
			case TokenKind::enDash: return new Punct(_eat().position, Punct::Type::enDash);
			case TokenKind::emDash: return new Punct(_eat().position, Punct::Type::emDash);

			case TokenKind::quoteOpen: return new Punct(_eat().position, Punct::Type::quoteOpen);
			case TokenKind::quoteClose: return new Punct(_eat().position, Punct::Type::quoteClose);
			case TokenKind::squoteOpen: return new Punct(_eat().position, Punct::Type::squoteOpen);
			case TokenKind::squoteClose: return new Punct(_eat().position, Punct::Type::squoteClose);

			case TokenKind::accountTag: return new AccountTag(_get(0).position, _eat().content);
			case TokenKind::hashTag: return new HashTag(_get(0).position, _eat().content);

			default: {
				return nullptr;
			}
		}
	}

	Node* _parseParagraph() {
		const Token& token = _get(0);

		Paragraph* paragraph = new Paragraph(token.position);

		while (_isBound(0) && _isParagraph()) {
			Node* node = _parseInline();

			if (node == nullptr) {
				if (_get(0).kind == TokenKind::newline) {
					if (_isBound(1) && _get(1).kind == TokenKind::indentOpen) {
						_advance(1);

						Node* indent = _parseIndent();
						if (indent == nullptr) {
							delete paragraph;
							return nullptr;
						}

						paragraph->children.append(indent);
						continue;
					}

					if (_isBound(1) && _get(1).kind == TokenKind::indentClose) {
						_advance(1);
						continue;
					}

					const Token& token = _eat();
					paragraph->children.append(new Space(token.position));
					continue;
				}

				continue;
			}

			paragraph->children.append(node);
		}

		if (_get(0).kind == TokenKind::newlinePlus) {
			_advance(1);
		}

		return paragraph;
	}

	Node* _parseHeading() {
		const Token& token = _eat();
		Heading* heading = new Heading(token.position);

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
						Node* subtitle = _parseHeading();
						if (subtitle == nullptr) {
							delete heading;
							return nullptr;
						}

						Heading* title = heading;

						heading = new Heading(token.position);
						heading->type = title->type;
						heading->children.append(title);

						title->type = Heading::Type::title;
						static_cast<Heading*>(subtitle)->type = Heading::Type::subtitle;

						heading->children.append(subtitle);
					}

					break;
				}

				delete heading;
				return _expect("newline or block");
			}

			heading->children.append(node);
		}

		return heading;
	}

	Node* _parseIndent() {
		const Token& token = _eat();
		Indent* indent = new Indent(token.position);

		while (_isBound(0) && _get(0).kind != TokenKind::indentClose) {
			Node* node = _parseBlock();

			if (node == nullptr) {
				delete indent;
				return nullptr;
			}

			indent->children.append(node);
		}

		if (!(_isBound(0) && _get(0).kind == TokenKind::indentClose)) {
			return _expect("indent pop");
		}

		_advance(1);

		if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
			_eat();
		}

		return indent;
	}

	Node* _parsePageBreak() {
		Node* node = new PageBreak(_eat().position);

		if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
			_advance(1);
		}

		return node;
	}

	Node* _parseDinkus() {
		Node* node = new Dinkus(_eat().position);

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

							item->children.append(subnode);
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

			item->children.append(node);
		}

		return ItemResult::ok;
	}

	Node* _parseList(TokenKind tokenKind, NodeKind nodeKind) {
		List* list = new List(_get(0).position, nodeKind);

		while (_isBound(0) && _get(0).kind == tokenKind) {
			Item* item = new Item(_eat().position);
			list->children.append(item);

			ItemResult result = _parseItem(list, item);

			switch (result) {
				case ItemResult::ok: break;
				case ItemResult::error: 
					delete list;
					return nullptr;
				case ItemResult::earlyExit: goto listEnd;
			}
		}

		listEnd:

		return list;
	}

	Node* _parseTodoList() {
		List* list = new List(_get(0).position, NodeKind::todoList);

		while (_isBound(0) && _get(0).kind == TokenKind::checkbox) {
			const Token& token = _eat();
			TodoItem* item = new TodoItem(token.position);
			list->children.append(item);

			switch (token.content.get(1)) {
				case ' ': item->state = TodoItem::State::todo; break;
				case 'v': item->state = TodoItem::State::done; break;
				case 'x': item->state = TodoItem::State::cancelled; break;
			}

			ItemResult result = _parseItem(list, item);

			switch (result) {
				case ItemResult::ok: break;
				case ItemResult::error:
					delete list;
					return nullptr;
				case ItemResult::earlyExit: goto listEnd;
			}
		}

		listEnd:

		return list;
	}

	Node* _checkTableRow(Node* node, Row::Type type) {
		// content only table
		if (type == Row::Type::header) {
			for (unsigned int i = 0; i < node->children.size(); i += 1) {
				static_cast<Row*>(node->children.get(i))->type = Row::Type::content;
			}
		}

		return node;
	}

	Node* _parseTable() {
		Table* table = new Table(_get(0).position);

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
									table->alignments.append(Table::Alignment::left);
								}
								goto nextTeeCell;

							case TokenKind::teeCenter:
								_advance(1);
								if (type == Row::Type::header) {
									table->alignments.append(Table::Alignment::center);
								}
								goto nextTeeCell;

							case TokenKind::teeRight:
								_advance(1);
								if (type == Row::Type::header) {
									table->alignments.append(Table::Alignment::right);
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
								return table;
								
							default:
								return table; // early exit
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

			Row* row = new Row(token.position);
			row->type = type;
			table->children.append(row);

			while (_isBound(0)) {
				Cell* cell = new Cell(_get(0).position);

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
								return _checkTableRow(table, type);
								
							default:
								delete cell;
								return _checkTableRow(table, type); // early exit
						}
					}

					cell->children.append(node);
				}

				nextCell:
				row->children.append(cell);
			}

			nextRow:
			continue;
		}

		return _checkTableRow(table, type);
	}


	Node* _parseLink() {
		Link* link = new Link(_eat().position);

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			link->setResource(_eat().content);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::squareClose) {
			_advance(1);

			if (_isBound(2) && 
				_get(0).kind == TokenKind::parenOpen && 
				_get(1).kind == TokenKind::raw && 
				_get(2).kind == TokenKind::parenClose) {

				link->label = _get(1).content;

				_advance(3);
			}
		}

		return link;
	}

	Node* _parseView() {
		View* view = new View(_eat().position);

		if (_isBound(0) && _get(0).kind == TokenKind::squareOpen) {
			_advance(1);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			view->resource = _eat().content;
		}

		if (_isBound(0) && _get(0).kind == TokenKind::squareClose) {
			_advance(1);

			if (_isBound(2) && 
				_get(0).kind == TokenKind::parenOpen && 
				_get(1).kind == TokenKind::raw && 
				_get(2).kind == TokenKind::parenClose) {

				view->label = _get(1).content;

				_advance(3);
			}
		}

		return view;
	}

	Node* _parseEmoji() {
		const Token& token = _eat();
		return new Emoji(token.position, token.content);
	}

	Node* _parseDateTime() {
		const Token& token = _eat();
		return new DateTime(token.position, token.content);
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
			StringSlice filePath = _eat().content;
			String path = _documentPath + String("/");
			path.append(filePath.pointer(), filePath.size());

			if (_fileInclusion) {
				Parser parser;
				document = parser.parseFile(StringSlice(path.pointer(), path.size()));

				if (document != nullptr) {
					document->position = token.position;
				}

				parser._document = nullptr;
			} else {
				document = new Document();
				document->path = String(filePath.pointer(), filePath.size());
				document->position = token.position;
			}
		}

		if (_isBound(0) && _get(0).kind == TokenKind::squareClose) {
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
			ref = new Footnote(token.position, _eat().content);
		}

		if (_isBound(0) && _get(0).kind == TokenKind::parenClose) {
			_advance(1);
		}

		return ref;
	}

	// Node* _parseFootnoteDecl() {
	// 	FootnoteDecl* decl = new FootnoteDecl(_eat().position);
	//
	// 	if (_isBound(0) && _get(0).kind == TokenKind::squareOpen) {
	// 		_advance(1);
	// 	}
	//
	// 	if (_isBound(0) && _get(0).kind == TokenKind::raw) {
	// 		decl->resource = _eat().content;
	// 	}
	//
	// 	if (_isBound(0) && _get(0).kind == TokenKind::squareClose) {
	// 		_advance(1);
	// 	}
	//
	// 	while (_isBound(0)) {
	// 		Node* node = _parseInline();
	//
	// 		if (node == nullptr) {
	// 			if (_get(0).kind == TokenKind::newline) {
	// 				if (_isBound(1) && _get(1).kind == TokenKind::indentOpen) {
	// 					_advance(2);
	//
	// 					while (_isBound(0)) {
	// 						Node* subnode = _parseBlock();
	// 						if (subnode == nullptr) {
	// 							if (_get(0).kind == TokenKind::indentClose) {
	// 								_advance(1);
	// 								if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
	// 									_advance(1);
	// 								}
	// 								return decl;
	// 								break;
	// 							}
	//
	// 							delete decl;
	// 							return _expect("indent pop");
	// 						}
	//
	// 						decl->children.append(subnode);
	// 					}
	//
	// 					break;
	// 				}
	//
	// 				_advance(1);
	// 				break;
	// 			}
	// 			
	// 			if (_get(0).kind == TokenKind::newlinePlus) {
	// 				_advance(1);
	// 				return decl;
	// 			}
	// 		}
	//
	// 		decl->children.append(node);
	// 	}
	//
	// 	return decl;
	// }

	Node* _parseCode() {
		Code* code = new Code(_eat().position);

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			code->content = _eat().content;
		}

		if (_isBound(0) && _get(0).kind == TokenKind::backtick) {
			_advance(1);

			if (_isBound(2) && 
				_get(0).kind == TokenKind::backtick && 
				_get(1).kind == TokenKind::raw && 
				_get(2).kind == TokenKind::backtick) {

				code->label = code->content;
				code->content = _get(1).content;

				_advance(3);
			}
		}

		return code;
	}

	Node* _parseCodeBlock() {
		CodeBlock* codeBlock = new CodeBlock(_eat().position);

		if (_isBound(0) && _get(0).kind == TokenKind::text) {
			codeBlock->label = _eat().content;
		}

		if (_isBound(0) && _get(0).kind == TokenKind::raw) {
			codeBlock->content = _eat().content;
		}

		if (_isBound(0) && _get(0).kind == TokenKind::fenceClose) {
			_advance(1);

			if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
				_advance(1);
			}
		}

		return codeBlock;
	}

	Node* _parseBlockquote() {
		const Token& token = _eat();
		Blockquote* indent = new Blockquote(token.position);

		while (_isBound(0) && _get(0).kind != TokenKind::blockquoteClose) {
			Node* node = _parseBlock();

			if (node == nullptr) {
				delete indent;
				return nullptr;
			}

			indent->children.append(node);
		}

		if (!(_isBound(0) && _get(0).kind == TokenKind::blockquoteClose)) {
			return _expect("blockquote pop");
		}

		_advance(1);

		if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
			_eat();
		}

		return indent;
	}

	Node* _parseAdmon() {
		const Token& token = _eat();
		Admon* admon = new Admon(token.position, token.content);

		while (_isBound(0)) {
			Node* node = _parseInline();

			if (node == nullptr) {
				if (_get(0).kind == TokenKind::newline) {
					if (_isBound(1) && _get(1).kind == TokenKind::indentOpen) {
						admon->children.append(new Space(_get(0).position));
						_advance(2);

						while (_isBound(0)) {
							Node* subnode = _parseBlock();
							if (subnode == nullptr) {
								if (_get(0).kind == TokenKind::indentClose) {
									_advance(1);
									if (_isBound(0) && (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus)) {
										_advance(1);
									}
									return admon;
									break;
								}

								delete admon;
								return _expect("indent pop");
							}

							admon->children.append(subnode);
						}

						break;
					}

					_advance(1);
					break;
				}
				
				if (_get(0).kind == TokenKind::newlinePlus) {
					_advance(1);
					return admon;
				}
			}

			admon->children.append(node);
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
			_annotations.append(annotation);
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
				return true;

			default: 
				return false;
		}
	}

	Node* _parseBlock() {
		switch (_get(0).kind) {
			case TokenKind::comment:
			case TokenKind::text:

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
				return _parseParagraph();

			case TokenKind::head1:
			case TokenKind::head2:
			case TokenKind::head3:
				return _parseHeading();

			case TokenKind::indentOpen:
				return _parseIndent();

			case TokenKind::pageBreak:
				return _parsePageBreak();

			case TokenKind::dinkus:
				return _parseDinkus();

			case TokenKind::question:
				return _parseInclude();

			case TokenKind::bullet:
				return _parseList(TokenKind::bullet, NodeKind::list);

			case TokenKind::index:
				return _parseList(TokenKind::index, NodeKind::numberedList);

			case TokenKind::checkbox:
				return _parseTodoList();

			case TokenKind::pipe:
				return _parseTable();

			case TokenKind::fenceOpen:
				return _parseCodeBlock();

			case TokenKind::blockquoteOpen:
				return _parseBlockquote();

			case TokenKind::admon:
				return _parseAdmon();

			default:
				return nullptr;
		}
	}

private:
	Slice<Token> _tokens;

	unsigned int _tokenIndex;

	Document* _document;

	String _documentPath;

	bool _fileInclusion;

	Array<Annotation> _annotations;
};

}
