#pragma once

#include "Gularen/Frontend/Node.h"

namespace Gularen {

class Parser {
public:
	Slice<Node*> parse(StringSlice content) {
		Lexer lexer;
		_tokens = lexer.parse(content);
		_tokenIndex = 0;

		while (_isBound(0)) {
			Node* node = _parseBlock();
			if (node == nullptr) {
				return {};
			}
			_nodes.append(node);
		}

		return Slice<Node*>(_nodes.pointer(), _nodes.size());
	}

private:
	decltype(nullptr) _wrong(StringSlice message) {
		printf("[ParsingError] %.*s\n", message.size(), message.pointer());
		return nullptr;
	}

	decltype(nullptr) _expect(StringSlice message) {
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

	Node* _parseComment() {
		const Token& token = _eat();
		return new Comment(token.position, token.value);
	}

	Node* _parseText() {
		const Token& token = _eat();
		return new Text(token.position, token.value);
	}

	Node* _parseInline() {
		switch (_get(0).kind) {
			case TokenKind::comment: return _parseComment();

			case TokenKind::text: return _parseText();

			case TokenKind::asterisk: return _parseStyle(Style::Type::bold);
			case TokenKind::underscore: return _parseStyle(Style::Type::italic);
			case TokenKind::backtick: return _parseStyle(Style::Type::monospaced);

			case TokenKind::lineBreak: return new LineBreak(_eat().position);

			default: {
				return nullptr;
			}
		}
	}

	Node* _parseParagraph() {
		const Token& token = _get(0);

		Paragraph* paragraph = new Paragraph(token.position);

		while (_isBound(0)) {
			Node* node = _parseInline();

			if (node == nullptr) {
				if (_get(0).kind == TokenKind::newlinePlus) {
					_advance(1);
					break;
				}

				if (_get(0).kind == TokenKind::newline) {
					if (_isBound(1) && _get(1).kind == TokenKind::indentPush) {
						_advance(1);

						Node* indent = _parseIndent();
						if (indent == nullptr) {
							delete paragraph;
							return nullptr;
						}

						paragraph->children.append(indent);
						continue;
					}

					if (_isBound(1) && _get(1).kind == TokenKind::indentPop) {
						_advance(1);
						continue;
					}

					const Token& token = _eat();
					paragraph->children.append(new Space(token.position));
					continue;
				}

				if (!_isParagraph()) {
					break;
				}
				
				delete paragraph;
				return _expect("newline+ or block");
			}

			paragraph->children.append(node);
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

		while (_isBound(0) && _get(0).kind != TokenKind::indentPop) {
			Node* node = _parseBlock();

			if (node == nullptr) {
				delete indent;
				return nullptr;
			}

			indent->children.append(node);
		}

		if (!(_isBound(0) && _get(0).kind == TokenKind::indentPop)) {
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
					if (_isBound(1) && _get(1).kind == TokenKind::indentPush) {
						_advance(2);

						while (_isBound(0)) {
							Node* subnode = _parseBlock();
							if (subnode == nullptr) {
								if (_get(0).kind == TokenKind::indentPop) {
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

				_expect("newline");
				return ItemResult::error;
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

			switch (token.value.get(1)) {
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

			if (_isBound(0) && (_get(0).kind == TokenKind::teeLeft || _get(0).kind == TokenKind::teeCenter || _get(0).kind == TokenKind::teeRight)) {
				while (_isBound(0)) {
					while (_isBound(0)) {
						switch (_get(0).kind) {
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
				row->children.append(cell);

				while (_isBound(0)) {
					Node* node = _parseInline();
					if (node == nullptr) {
						switch (_get(0).kind) {
							case TokenKind::pipe:
								_advance(1);
								goto nextCell;

							case TokenKind::newline:
								_advance(1);
								goto nextRow;

							case TokenKind::newlinePlus:
								_advance(1);
								return _checkTableRow(table, type);
								
							default:
								return _checkTableRow(table, type); // early exit
						}
					}

					cell->children.append(node);
				}

				nextCell:
				continue;
			}

			nextRow:
			continue;
		}

		return _checkTableRow(table, type);
	}

	bool _isParagraph() {
		switch (_get(0).kind) {
			case TokenKind::comment:
			case TokenKind::text:

			case TokenKind::asterisk:
			case TokenKind::underscore:
			case TokenKind::backtick:
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
				return _parseParagraph();

			case TokenKind::head1:
			case TokenKind::head2:
			case TokenKind::head3:
				return _parseHeading();

			case TokenKind::indentPush:
				return _parseIndent();

			case TokenKind::pageBreak:
				return _parsePageBreak();

			case TokenKind::dinkus:
				return _parseDinkus();

			case TokenKind::bullet:
				return _parseList(TokenKind::bullet, NodeKind::list);

			case TokenKind::index:
				return _parseList(TokenKind::index, NodeKind::numberedList);

			case TokenKind::checkbox:
				return _parseTodoList();

			case TokenKind::pipe:
				return _parseTable();

			default:
				return nullptr;
		}
	}

private:
	Slice<Token> _tokens;

	unsigned int _tokenIndex;

	Array<Node*> _nodes;
};

}
