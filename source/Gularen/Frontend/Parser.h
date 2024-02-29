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
		printf("[ParsingError] unxpected token, expect %.*s\n", message.size(), message.pointer());
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
				if (_isBlock()) {
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
		Node* heading = nullptr;

		switch (token.kind) {
			case TokenKind::head1:
				heading = new Subsection(token.position);
				break;

			case TokenKind::head2:
				heading = new Section(token.position);
				break;

			case TokenKind::head3:
				heading = new Chapter(token.position);
				break;

			default: return nullptr;
		}

		
		while (_isBound(0)) {
			Node* node = _parseInline();
			if (node == nullptr) {
				if (_get(0).kind == TokenKind::newline || _get(0).kind == TokenKind::newlinePlus) {
					_advance(1);
					break;
				}

				if (_get(0).kind == TokenKind::head1) {
					if (!(_isBound(1) && _get(1).kind == TokenKind::text)) {

					}
				}

				delete heading;
				return _expect("newline or block");
			}

			heading->children.append(node);
		}

		return heading;
	}

	bool _isBlock() {
		switch (_get(0).kind) {
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

			default: {
				StringSlice kind = toStringSlice(_get(0).kind);
				printf("[Parser.parseBlock] unhandled token %.*s\n", kind.size(), kind.pointer());
				return nullptr;
			}
		}
	}

private:
	Slice<Token> _tokens;

	unsigned int _tokenIndex;

	Array<Node*> _nodes;
};

}
