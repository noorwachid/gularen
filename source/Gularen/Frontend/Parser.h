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

	bool _isBlock() {
		switch (_get(0).kind) {
			case TokenKind::head1:
			case TokenKind::head2:
			case TokenKind::head3:

			case TokenKind::indentPush:
			case TokenKind::indentPop:
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
