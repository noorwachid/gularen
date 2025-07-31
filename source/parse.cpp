#include "parse.hpp"
#include "lexeme.hpp"

struct Point {
	int index;
	Range range;
};

struct Parser {
	Array<Token> _tokens;
	int _index;

	Parser(String const& source) {
		_tokens = lexeme(source);
		_index = 0;
	}
	Node* parse() {
		DocumentNode* document = new DocumentNode();
		document->kind = NodeKind_document;
		while (_has()) {
			Node* node = _parseBlock();
			if (node == nullptr) {
				continue;
			}
			document->children.append(node);
		}
		return document;
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
			case TokenKind_openref:
			case TokenKind_footnote:
				return _parseParagraph();
			default:
				_advance();
				return nullptr;
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
			if (_get().kind == TokenKind_subheading) {
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
		if (_is(TokenKind_lang)) {
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
			if (_get().kind == TokenKind_newline) {
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
				case TokenKind_openref:
				case TokenKind_footnote: {
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
					goto end;
			}
		}
		end:
		_range(paragraph);
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
			case TokenKind_openref: {
				ResourceNode* resource = new ResourceNode();
				switch (_get().content[0]) {
					case '[':
						resource->kind = NodeKind_link;
						break;
					case '!':
						resource->kind = NodeKind_view;
						break;
					case '^':
						resource->kind = NodeKind_citation;
						break;
				}
				resource->range = _get().range;
				_advance();
				if (_is(TokenKind_ref)) {
					resource->source = _get().content;
					_advance();
				}
				if (_is(TokenKind_quotedref)) {
					resource->source = _parseQuotedString(_get().content);
					_advance();
				}
				if (_is(TokenKind_closeref)) {
					resource->range.end = _get().range.end;
					_advance();
				}
				if (_is(TokenKind_openlabel)) {
					_advance();
					while (_has()) {
						if (_get().kind == TokenKind_closelabel) {
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
				if (_is(TokenKind_openlabel)) {
					_advance();
					while (_has()) {
						if (_get().kind == TokenKind_closelabel) {
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
		if (_is(TokenKind_lang)) {
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
	bool _is(TokenKind kind) {
		return _index < _tokens.size() && _tokens[_index].kind == kind;
	}
	Token const& _get() {
		return _tokens[_index];
	}
	void _advance() {
		_index++;
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

Node* parse(String const& source) {
	Parser parser(source);
	return parser.parse();
}

