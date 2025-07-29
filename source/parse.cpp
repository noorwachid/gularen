#include "parse.hpp"
#include "lexeme.hpp"
#include <stdio.h>

struct Point {
	int index;
	Range range;
};

struct Parser {
	Array<Token> _tokens;
	Point _point;

	Parser(String const& source) {
		_tokens = lexeme(source);
		_point.index = 0;
		_point.range.start.line = 0;
		_point.range.start.column = 0;
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
			case TokenKind_text:
			case TokenKind_asterisk:
			case TokenKind_underscore:
				return _parseParagraph();
			case TokenKind_indent:
				return _parseQuote();
			default:
				_advance();
				return nullptr;
		}
	}

	Node* _parseQuote() {
		Point point = _point;
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

	Node* _parseParagraph() {
		Point point = _point;
		HierarchyNode* paragraph = new HierarchyNode();
		paragraph->kind = NodeKind_paragraph;

		while (_has()) {
			switch (_get().kind) {
				case TokenKind_text:
				case TokenKind_asterisk:
				case TokenKind_underscore: {
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

	Node* _parseLine() {
		switch (_get().kind) {
			case TokenKind_asterisk:
				return _parseSurrounding(NodeKind_strong);
			case TokenKind_underscore:
				return _parseSurrounding(NodeKind_emphasis);
			case TokenKind_text: {
				TextNode* text = new TextNode();
				text->kind = NodeKind_text;
				text->range = _get().range;
				text->content = _get().content;
				_advance();
				return text;
			}
			default:
				return nullptr;
		}
	}

	Node* _parseSurrounding(NodeKind kind) {
		Token token = _get();
		HierarchyNode* emphasis = new HierarchyNode();
		emphasis->kind = kind;
		_advance();
		while (_has()) {
			if (_get().kind == token.kind) {
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
		_range(emphasis);
		return emphasis;
	}

	bool _has() {
		return _point.index < _tokens.size();
	}
	bool _is(TokenKind kind) {
		return _point.index < _tokens.size() && _tokens[_point.index].kind == kind;
	}
	Token const& _get() {
		return _tokens[_point.index];
	}
	void _advance() {
		_point.index++;
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

