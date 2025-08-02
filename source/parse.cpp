#include "parse.hpp"
#include "lexeme.hpp"
#include "print.hpp"
#include "Collection/Disk.hpp"
#include <stdio.h>

struct Point {
	int index;
	Range range;
};

struct Parser {
	Array<Token> _tokens;
	int _index;
	String _path;
	String _directory;

	Parser(String const& source, String const& path) {
		int size = 0;
		for (int i = path.size() - 1; i >= 0; i--) {
			if (path[i] == '/') {
				size = i + 1;
				break;
			}
		}
		_path = path;
		_directory = path.slice(0, size);
		_tokens = lexeme(source);
		_index = 0;
	}
	DocumentNode* parseDocument() {
		if (_tokens.size() == 0) {
			return nullptr;
		}
		DocumentNode* document = new DocumentNode();
		document->kind = NodeKind_document;
		document->range = _get().range;
		document->path = _path;
		_parseMetadata(document);

		while (_has()) {
			Node* node = _parseBlock();
			if (node == nullptr) {
				// TODO: delete attribute items
				continue;
			}
			document->children.append(node);
		}
		if (document->children.size() != 0) {
			document->range.end = document->children[document->children.size() - 1]->range.end;
		}
		return document;
	}

	void _parseMetadata(DocumentNode* doc) {
		if (_hasNext(3) &&
			_getNext(0).kind == TokenKind_script &&
			_getNext(1).kind == TokenKind_symbol &&
			_getNext(2).kind == TokenKind_colon &&
			_getNext(3).kind == TokenKind_string) {
			while (_has()) {
				parseEntry:
				if (_hasNext(3) &&
					_getNext(0).kind == TokenKind_script &&
					_getNext(1).kind == TokenKind_symbol &&
					_getNext(2).kind == TokenKind_colon &&
					_getNext(3).kind == TokenKind_string) {
					doc->metadata.set(_getNext(1).content, _getNext(3).content);
					doc->range.end = _getNext(3).range.end;
					_advanceNext(3);

					if (_is(TokenKind_newline)) {
						_advance();
						goto parseEntry;
					}
					if (_is(TokenKind_newlines)) {
						_advance();
						return;
					}
					continue;
				}
				break;
			}
		}
	}

	Node* _parseBlock() {
		switch (_get().kind) {
			case TokenKind_indent:
				return _parseQuote();
			case TokenKind_heading:
				return _parseHeading();
			case TokenKind_bullet:
				return _parseList(TokenKind_bullet, NodeKind_list, NodeKind_item);
			case TokenKind_numberpoint:
				return _parseList(TokenKind_numberpoint, NodeKind_numberedlist, NodeKind_numbereditem);
			case TokenKind_checkbox:
				return _parseList(TokenKind_checkbox, NodeKind_checklist, NodeKind_checkitem);
			case TokenKind_thematicbreak:
				return _parseThematicBreak();
			case TokenKind_openfence:
				return _parseCode();
			case TokenKind_admon:
				return _parseAdmon();
			case TokenKind_pipe:
				return _parseTable();
			case TokenKind_script:
				return _parseScript();
			case TokenKind_text:
			case TokenKind_escape:
			case TokenKind_asterisk:
			case TokenKind_underscore:
			case TokenKind_backtick:
			case TokenKind_emoji:
			case TokenKind_linebreak:
			case TokenKind_hyphen:
			case TokenKind_endash:
			case TokenKind_emdash:
			case TokenKind_leftquote:
			case TokenKind_rightquote:
			case TokenKind_singleleftquote:
			case TokenKind_singlerightquote:
			case TokenKind_openbracket:
			case TokenKind_footnote:
			default:
				return _parseParagraph();
		}
	}

	Node* _parseQuote() {
		HierarchyNode* quote = new HierarchyNode();
		quote->kind = NodeKind_quote;
		_advance();

		while (_has()) {
			if (_get().kind == TokenKind_outdent) {
				_advance();
				goto end;
			}
			Node* node = _parseBlock();
			if (node == nullptr) {
				goto end;
			}
			quote->children.append(node);
		}
		end:
		_range(quote);
		return quote;
	}

	Node* _parseHeading() {
		Token token = _get();
		HierarchyNode* section = new HierarchyNode();
		_advance();
		switch (token.content.size()) {
			case 3:
				section->kind = NodeKind_chapter;
				break;
			case 2:
				section->kind = NodeKind_section;
				break;
			case 1:
				section->kind = NodeKind_subsection;
				break;
		}
		HierarchyNode* title = new HierarchyNode();
		title->kind = NodeKind_title;
		while (_has()) {
			if (_get().kind == TokenKind_newline ||
				_get().kind == TokenKind_newlines) {
				_advance();
				goto body;
			}
			if (_get().kind == TokenKind_colon) {
				_advance();
				HierarchyNode* subtitle = new HierarchyNode();
				subtitle->kind = NodeKind_subtitle;
				while (_has()) {
					if (_get().kind == TokenKind_newline ||
						_get().kind == TokenKind_newlines) {
						_advance();
						goto endSubtitle;
					}
					Node* node = _parseLine();
					if (node == nullptr) {
						goto endSubtitle;
					}
					subtitle->children.append(node);
				}
				endSubtitle:
				_range(subtitle);
				title->children.append(subtitle);
				goto body;
			}
			Node* node = _parseLine();
			if (node == nullptr) {
				goto body;
			}
			title->children.append(node);
		}
		body:
		_range(title);
		section->children.append(title);
		while (_has()) {
			if (_get().kind == TokenKind_heading &&
				_get().content.size() >= token.content.size()) {
				goto end;
			}
			Node* node = _parseBlock();
			if (node == nullptr) {
				goto end;
			}
			section->children.append(node);
		}
		end:
		_range(section);
		return section;
	}

	Node* _parseList(TokenKind pointKind, NodeKind listKind, NodeKind itemKind) {
		HierarchyNode* list = new HierarchyNode();
		list->kind = listKind;
		list->range = _get().range;

		while (_has()) {
			if (_get().kind != pointKind) {
				goto end;
			}
			HierarchyNode* item = nullptr;
			if (pointKind == TokenKind_checkbox) {
				CheckItemNode* checkitem = new CheckItemNode();
				checkitem->isChecked = _get().content[3] == 'x';
				item = checkitem;
			} else {
				item = new HierarchyNode();
			}
			item->kind = itemKind;
			item->range = _get().range;
			_advance();
			while (_has()) {
				if (_get().kind == TokenKind_newline ||
					_get().kind == TokenKind_newlines) {
					_advance();
					if (_has() && _get().kind == TokenKind_indent) {
						_advance();
						while (_has()) {
							if (_get().kind == TokenKind_outdent) {
								_advance();
								goto endItem;
							}
							Node* node = _parseBlock();
							item->children.append(node);
						}
					}
					goto endItem;
				}
				Node* node = _parseLine();
				if (node == nullptr) {
					goto endItem;
				}
				item->children.append(node);
			}
			endItem:
			if (item->children.size() != 0) {
				item->range.end = item->children[item->children.size() - 1]->range.end;
			}
			list->children.append(item);
		}
		end:
		if (list->children.size() != 0) {
			list->range.end = list->children[list->children.size() - 1]->range.end;
		}
		return list;
	}

	Node* _parseThematicBreak() {
		ContentNode* thematicbreak = new ContentNode();
		thematicbreak->kind = NodeKind_thematicbreak;
		thematicbreak->range = _get().range;
		thematicbreak->content = _get().content;
		_advance();
		return thematicbreak;
	}

	Node* _parseCode() {
		Token token = _get();
		CodeNode* code = new CodeNode();
		code->kind = NodeKind_fencedcode;
		code->range = token.range;
		_advance();
		if (_is(TokenKind_symbol)) {
			code->lang = _get().content;
			_advance();
		}
		if (_is(TokenKind_newline)) {
			_advance();
			if (_is(TokenKind_sources)) {
				code->content = _parseCodeSource(token.range.start, _get().content);
				_advance();
			}
			if (_is(TokenKind_closefence)) {
				code->range.end = _get().range.end;
				_advance();
			}
		}
		return code;
	}

	Node* _parseAdmon() {
		AdmonNode* admon = new AdmonNode();
		admon->kind = NodeKind_admon;
		admon->range = _get().range;

		_advance();
		if (_is(TokenKind_newline)) {
			_advance();
			Node* block = _parseBlock();
			if (block != nullptr) {
				admon->children.append(block);
				admon->range.end = block->range.end;
			}
			return admon;
		}
		while (_has()) {
			if (_get().kind == TokenKind_newline ||
				_get().kind == TokenKind_newlines) {
				break;
			}
			Node* node = _parseLine();
			if (node != nullptr) {
				admon->children.append(node);
				continue;
			}
			break;
		}
		if (admon->children.size() != 0) {
			admon->range.end = admon->children[admon->children.size() - 1]->range.end;
		}
		return admon;
	}

	Node* _parseTable() {
		TableNode* table = new TableNode();
		table->kind = NodeKind_table;
		table->range = _get().range;

		while (_has()) {
			parseRow:
			if (_get().kind == TokenKind_pipe) {
				if (_isNext(1, TokenKind_bar)) {
					while (_has()) {
						if (_get().kind == TokenKind_pipe) {
							table->range.end = _get().range.end;
							_advance();

							if (_has() && (_get().kind == TokenKind_newline || _get().kind == TokenKind_newlines)) {
								_advance();
								goto parseRow;
							}

							if (_is(TokenKind_bar)) {
								table->isHeadered = true;
								Alignment alignment = Alignment_left;
								Byte first = _get().content[0];
								Byte last = _get().content[_get().content.size() - 1];
								if (first == ':' && last == ':') {
									alignment = Alignment_center;
								} else if (last == ':') {
									alignment = Alignment_right;
								}
								table->range.end = _get().range.end;
								table->alignments.append(alignment);
								_advance();
							}
						}
					}
				}

				HierarchyNode* row = new HierarchyNode();
				row->kind = NodeKind_row;
				row->range = _get().range;
				table->children.append(row);

				while (_has()) {
					parseCell:
					if (_get().kind == TokenKind_pipe) {
						Range range = _get().range;
						_advance();
						if (_has()) {
							if (_get().kind == TokenKind_newline) {
								row->range.end = _get().range.end;
								_advance();
								goto parseRow;
							}
							if (_get().kind == TokenKind_newlines) {
								row->range.end = _get().range.end;
								_advance();
								goto end;
							}
						}

						HierarchyNode* cell = new HierarchyNode();
						cell->kind = NodeKind_cell;
						cell->range = range;
						row->children.append(cell);

						while (_has()) {
							if (_get().kind == TokenKind_pipe || 
								_get().kind == TokenKind_newline ||
								_get().kind == TokenKind_newlines) {
								goto parseCell;
							}

							Node* node = _parseLine();
							if (node == nullptr) {
								goto parseCell;
							}
							cell->children.append(node);
							cell->range.end = node->range.end;
							row->range.end = node->range.end;
						}
					}
					break;
				}
			}
			break;
		}
		end:
		if (table->children.size() != 0) {
			table->range.end = table->children[table->children.size() - 1]->range.end;
		}
		return table;
	}

	Node* _parseScript() {
		Token token = _get();
		if (_hasNext(2) && 
			_getNext(1).kind == TokenKind_func && _getNext(1).content == "include" && 
			_getNext(2).kind == TokenKind_string) {
			String path = _directory;
			path.append(_parseQuotedString(_getNext(2).content));
			_advanceNext(2);
			DocumentNode* doc = parseFile(path);
			return doc;
		}
		if (_hasNext(2) && 
			_getNext(1).kind == TokenKind_func &&
			(_getNext(2).kind == TokenKind_newline || _getNext(2).kind == TokenKind_newlines)) {
			FuncNode* func = new FuncNode();
			func->kind = NodeKind_func;
			func->range = token.range;
			func->range.end = _getNext(1).range.end;
			func->symbol = _getNext(1).content;
			_advanceNext(2);
			return func;
		}
		if (_hasNext(3) && 
			_getNext(1).kind == TokenKind_func &&
			_getNext(2).kind == TokenKind_string &&
			(_getNext(3).kind == TokenKind_newline || _getNext(3).kind == TokenKind_newlines)) {
			FuncNode* func = new FuncNode();
			func->kind = NodeKind_func;
			func->range = token.range;
			func->range.end = _getNext(1).range.end;
			func->symbol = _getNext(1).content;
			func->arguments.set("0", _getNext(2).content);
			_advanceNext(3);
			return func;
		}
		if (_hasNext(3) && 
			_getNext(1).kind == TokenKind_func &&
			_getNext(2).kind == TokenKind_openbrace && 
			_getNext(3).kind == TokenKind_newline) {
			FuncNode* func = new FuncNode();
			func->kind = NodeKind_func;
			func->range = token.range;
			func->symbol = _getNext(1).content;
			_advanceNext(3);
			while (_has()) {
				if (_get().kind == TokenKind_closebrace) {
					func->range.end = _get().range.end;
					_advance();
					if (_is(TokenKind_newline) || _is(TokenKind_newlines)) {
						_advance();
					}
					return func;
				}
				if (_hasNext(3) && 
					_getNext(0).kind == TokenKind_symbol &&
					_getNext(1).kind == TokenKind_colon &&
					_getNext(2).kind == TokenKind_string &&
					_getNext(3).kind == TokenKind_newline) {
					func->arguments.set(_getNext(0).content, _getNext(2).content);
					_advanceNext(3);
					continue;
				}
				break;
			}
		}
		return _parseParagraph();
	}

	String _parseCodeSource(Position position, String const& content) {
		String parsedContent;
		int index = 0;
		while (index < content.size()) {
			if (content[index] == '\t') {
				int depth = 0;
				while (index < content.size()) {
					if (content[index] == '\t') {
						depth++;
						index++;
						if (depth == position.column) {
							goto consume;
						}
						continue;
					}
					break;
				}
			}
			consume:
			while (index < content.size()) {
				if (content[index] == '\n') {
					parsedContent.append(1, content.items() + index);
					index++;
					break;
				}
				parsedContent.append(1, content.items() + index);
				index++;
			}
		}
		return parsedContent;
	}

	Node* _parseParagraph() {
		HierarchyNode* paragraph = new HierarchyNode();
		paragraph->kind = NodeKind_paragraph;

		while (_has()) {
			switch (_get().kind) {
				case TokenKind_text:
				case TokenKind_escape:
				case TokenKind_asterisk:
				case TokenKind_underscore:
				case TokenKind_backtick:
				case TokenKind_emoji:
				case TokenKind_linebreak:
				case TokenKind_hyphen:
				case TokenKind_endash:
				case TokenKind_emdash:
				case TokenKind_leftquote:
				case TokenKind_rightquote:
				case TokenKind_singleleftquote:
				case TokenKind_singlerightquote:
				case TokenKind_openbracket:
				case TokenKind_footnote: {
				case TokenKind_hashtag:
					Node* node = _parseLine();
					if (node == nullptr) {
						goto end;
					}
					paragraph->children.append(node);
					break;
				}
				case TokenKind_newline: 
					_advance();
					break;
				case TokenKind_newlines:
					_advance();
					goto end;
				default:
					paragraph->children.append(_createContent(NodeKind_text));
					break;
			}
		}
		end:
		_range(paragraph);
		if (paragraph->children.size() == 1 && paragraph->children[0]->kind == NodeKind_view) {
			Node* node = paragraph->children[0];
			paragraph->children.set(0, nullptr);
			delete paragraph;
			return node;
		}
		return paragraph;
	}

	Node* _createContent(NodeKind kind) {
		ContentNode* text = new ContentNode();
		text->kind = kind;
		text->range = _get().range;
		text->content = _get().content;
		_advance();
		return text;
	}

	Node* _create(NodeKind kind) {
		Node* text = new Node();
		text->kind = kind;
		text->range = _get().range;
		_advance();
		return text;
	}

	Node* _parseLine() {
		switch (_get().kind) {
			case TokenKind_text:
				return _createContent(NodeKind_text);
			case TokenKind_escape:
				return _parseEscape();
			case TokenKind_asterisk:
				return _parseSurrounding(NodeKind_strong);
			case TokenKind_underscore:
				return _parseSurrounding(NodeKind_emphasis);
			case TokenKind_backtick:
				return _parseLineCode();
			case TokenKind_emoji:
				return _parseEmoji();
			case TokenKind_linebreak:
				return _parseLineBreak();
			case TokenKind_hyphen:
				return _create(NodeKind_hyphen);
			case TokenKind_endash:
				return _create(NodeKind_endash);
			case TokenKind_emdash:
				return _create(NodeKind_emdash);
			case TokenKind_leftquote:
				return _create(NodeKind_leftquote);
			case TokenKind_rightquote:
				return _create(NodeKind_rightquote);
			case TokenKind_singleleftquote:
				return _create(NodeKind_singleleftquote);
			case TokenKind_singlerightquote:
				return _create(NodeKind_singlerightquote);
			case TokenKind_openbracket: {
				ResourceNode* resource = new ResourceNode();
				switch (_get().content[0]) {
					case '[':
						resource->kind = NodeKind_link;
						break;
					case '!':
						resource->kind = NodeKind_view;
						break;
					case '^':
						resource->kind = NodeKind_cite;
						break;
				}
				resource->range = _get().range;
				_advance();
				if (_is(TokenKind_ref)) {
					resource->source = _get().content;
					_advance();
				}
				if (_is(TokenKind_string)) {
					resource->source = _parseQuotedString(_get().content);
					_advance();
				}
				if (_is(TokenKind_closebracket)) {
					resource->range.end = _get().range.end;
					_advance();
				}
				if (_is(TokenKind_openparen)) {
					_advance();
					while (_has()) {
						if (_get().kind == TokenKind_closeparen) {
							resource->range.end = _get().range.end;
							_advance();
							break;
						}
						Node* node = _parseLine();
						if (node == nullptr) {
							break;
						}
						resource->children.append(node);
					}
				}
				return resource;
			}
			case TokenKind_footnote: {
				HierarchyNode* footnote = new HierarchyNode();
				footnote->kind = NodeKind_footnote;
				footnote->range = _get().range;
				_advance();
				if (_is(TokenKind_openparen)) {
					_advance();
					while (_has()) {
						if (_get().kind == TokenKind_closeparen) {
							footnote->range.end = _get().range.end;
							_advance();
							break;
						}
						Node* node = _parseLine();
						if (node == nullptr) {
							break;
						}
						footnote->children.append(node);
					}
				}
				return footnote;
			}
			case TokenKind_hashtag: {
				ContentNode* tag = new ContentNode();
				tag->kind = NodeKind_hashtag;
				tag->range = _get().range;
				tag->content = _get().content.slice(1, _get().content.size() - 1);
				_advance();
				return tag;
			}
			default:
				return nullptr;
		}
	}

	Node* _parseEscape() {
		Token token = _get();
		ContentNode* content = new ContentNode();
		content->kind = NodeKind_text;
		content->range = token.range;
		content->content = token.content.slice(1, token.content.size() - 1);
		_advance();
		return content;
	}

	Node* _parseSurrounding(NodeKind kind) {
		Token token = _get();
		HierarchyNode* emphasis = new HierarchyNode();
		emphasis->kind = kind;
		emphasis->range = token.range;
		_advance();
		while (_has()) {
			if (_get().kind == token.kind) {
				emphasis->range.end = _get().range.end;
				_advance();
				goto end;
			}
			Node* node = _parseLine();
			if (node == nullptr) {
				goto end;
			}
			emphasis->children.append(node);
		}
		end:
		return emphasis;
	}

	Node* _parseLineCode() {
		CodeNode* code = new CodeNode();
		code->kind = NodeKind_code;
		code->range = _get().range;
		_advance();
		if (_is(TokenKind_symbol)) {
			String lang = _get().content;
			code->content = lang;
			code->range.end = _get().range.end;
			_advance();
			if (_is(TokenKind_backtick)) {
				code->range.end = _get().range.end;
				Range firstRange = _get().range;
				_advance();
				if (_is(TokenKind_backtick)) {
					Range secondRange = _get().range;
					code->range.end = _get().range.end;
					_advance();
					if (_is(TokenKind_source)) {
						String content = _get().content;
						code->range.end = _get().range.end;
						_advance();
						if (_is(TokenKind_backtick)) {
							code->lang = lang;
							code->content = content;
							code->range.end = _get().range.end;
							_advance();
						}
					}
				}
			}
		}
		if (_is(TokenKind_source)) {
			String content = _get().content;
			code->range.end = _get().range.end;
			_advance();
			if (_is(TokenKind_backtick)) {
				code->content = content;
				code->range.end = _get().range.end;
				_advance();
			}
		}
		return code;
	}

	Node* _parseEmoji() {
		Token token = _get();
		ContentNode* emoji = new ContentNode();
		emoji->kind = NodeKind_emoji;
		emoji->content = token.content.slice(1, token.content.size() - 2);
		emoji->range = token.range;
		_advance();
		return emoji;
	}

	Node* _parseLineBreak() {
		Token token = _get();
		Node* linebreak = new Node();
		linebreak->kind = NodeKind_linebreak;
		linebreak->range = token.range;
		_advance();
		return linebreak;
	}

	bool _has() {
		return _index < _tokens.size();
	}
	bool _hasNext(int offset) {
		return _index + offset < _tokens.size();
	}
	bool _is(TokenKind kind) {
		return _index < _tokens.size() && _tokens[_index].kind == kind;
	}
	bool _isNext(int offset, TokenKind kind) {
		return _index + offset < _tokens.size() && _tokens[_index + offset].kind == kind;
	}
	Token const& _get() {
		return _tokens[_index];
	}
	Token const& _getNext(int offset) {
		return _tokens[_index + offset];
	}
	void _advance() {
		_index++;
	}
	void _advanceNext(int offset) {
		_index += offset + 1;
	}
	String _parseQuotedString(String const& quoted) {
		String native;
		int size = quoted.size() - 1;
		for (int i = 1; i < size; i++) {
			switch (quoted[i]) {
				case '\\':
					native.append(1, quoted.items() + i + 1);
					i++;
					break;
				default:
					native.append(1, quoted.items() + i);
					break;
			}
		}
		return native;
	}

	void _range(HierarchyNode* node) {
		if (node->children.size() == 0) {
			return;
		}

		node->range.start = node->children[0]->range.start;
		node->range.end = node->children[node->children.size() - 1]->range.end;
	}
};

DocumentNode* parseFile(String const& path) {
	Parser parser(readFile(path), path);
	return parser.parseDocument();
}

DocumentNode* parse(String const& source) {
	Parser parser(source, "");
	return parser.parseDocument();
}

