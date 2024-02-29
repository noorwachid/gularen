#pragma once

#include "Gularen/Library/Array.h"
#include "Gularen/Library/Slice.h"
#include "Gularen/Library/StringSlice.h"

namespace Gularen {

enum class TokenKind {
	comment,

	text,

	newline,
	newlinePlus,

	indentInc,
	indentDec,

	asterisk,
	underscore,
	backtick,

	head3,
	head2,
	head1,

	indentPush,
	indentPop,

	lineBreak,
	pageBreak,

	dinkus,

	bullet,
	index,
	checkbox,
};

StringSlice toStringSlice(TokenKind kind) {
	switch (kind) {
		case TokenKind::comment: return "comment";

		case TokenKind::text: return "text";

		case TokenKind::newline: return "newline";
		case TokenKind::newlinePlus: return "newline+";

		case TokenKind::indentInc: return "->";
		case TokenKind::indentDec: return "<-";

		case TokenKind::asterisk: return "asterisk";
		case TokenKind::underscore: return "underscore";
		case TokenKind::backtick: return "backtick";

		case TokenKind::head3: return "head3";
		case TokenKind::head2: return "head2";
		case TokenKind::head1: return "head1";

		case TokenKind::indentPush: return "indentPush";
		case TokenKind::indentPop: return "indentPop";

		case TokenKind::lineBreak: return "lineBreak";
		case TokenKind::pageBreak: return "pageBreak";
		case TokenKind::dinkus: return "dinkus";

		case TokenKind::bullet: return "bullet";
		case TokenKind::index: return "index";
		case TokenKind::checkbox: return "checkbox";
	}
}

struct Position {
	unsigned int line;
	unsigned int column;
};

struct Token {
	Position position;
	TokenKind kind;
	StringSlice value;
};

class Lexer {
public:
	Slice<Token> parse(StringSlice content) {
		_content = content;
		_contentIndex = 0;
		_indentLevel = 0;

		_consumeIndent();

		while (_isBound(0)) {
			switch (_get(0)) {
				case '~':
					_consumeComment();
					break;

				case '*': 
					if (_isBound(2) && _get(1) == '*' && _get(2) == '*') {
						_append(TokenKind::dinkus, _contentIndex, 3); 
						_advance(3);
						break;
					}

					_append(TokenKind::asterisk); 
					_advance(1);
					break;

				case '_': 
					_append(TokenKind::underscore); 
					_advance(1);
					break;

				case '`': 
					_append(TokenKind::backtick); 
					_advance(1);
					break;

				case '>': 
					if (_isBound(1) && _get(1) == '>') {
						if (_isBound(2) && _get(2) == '>') {
							if (_isBound(3) && _get(3) == ' ') {
								_append(TokenKind::head3, _contentIndex, 3);
								_advance(4);
								break;
							}
						}

						if (_isBound(2) && _get(2) == ' ') {
							_append(TokenKind::head2, _contentIndex, 2);
							_advance(3);
						}
						break;
					}

					if (_isBound(1) && _get(1) == ' ') {
						_append(TokenKind::head1, _contentIndex, 1);
						_advance(2);
						break;
					}
					break;

				case '<': 
					if (_isBound(1) && _get(1) == '<') {
						if (_isBound(2) && _get(2) == '<') {
							_append(TokenKind::pageBreak, _contentIndex, 3);
							_advance(3);
							break;
						}

						_append(TokenKind::lineBreak, _contentIndex, 2);
						_advance(2);
						break;
					}

					_advance(1);
					break;

				case '-':
					if (_isBound(1) && _get(1) == ' ') {
						_append(TokenKind::bullet, _contentIndex, 1);
						_advance(2);
						break;
					}

					_consumeText();
					break;

				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					_consumeIndex();
					break;

				case '\n': {
					unsigned int count = 0;

					while (_isBound(0) && _get(0) == '\n') {
						count += 1;
						_advance(1);
					}

					if (count == 1) {
						_append(TokenKind::newline);
						_consumeIndent();
						break;
					}

					_append(TokenKind::newlinePlus);
					_consumeIndent();
					break;
				}

				default: 
					_consumeText(); 
					break;
			}
		}

		if (_tokens.size() != 0) {
			switch (_tokens.get(_tokens.size() - 1).kind) {
				case TokenKind::newlinePlus: 
					break;
				case TokenKind::newline: 
					_tokens.get(_tokens.size() - 1).kind = TokenKind::newlinePlus;
					break;
				default: 
					_append(TokenKind::newlinePlus);
					break;
			}
		}


		return Slice<Token>(_tokens.pointer(), _tokens.size());
	}

private:
	bool _isBound(unsigned int offset) const {
		return _contentIndex + offset < _content.size();
	}

	void _advance(unsigned int offset) {
		_contentIndex += offset;
	}

	char _get(unsigned int offset) const {
		return _content.get(_contentIndex + offset);
	}

	void _append(TokenKind kind, unsigned int index = 0, unsigned int size = 0) {
		Token token;
		token.position.line = 0;
		token.position.column = 0;
		token.kind = kind;
		token.value = _content.cut(index, size);
		_tokens.append(static_cast<Token&&>(token));
	}

	void _consumeIndent() {
		unsigned int indentLevel = 0;
		while (_isBound(0) && _get(0) == '\t') {
			indentLevel += 1;
			_advance(1);
		}

		if (_indentLevel == indentLevel) {
			return;
		}

		if (_indentLevel < indentLevel) {
			while (_indentLevel < indentLevel) {
				_append(TokenKind::indentPush);
				_indentLevel += 1;
			}
		}

		if (_indentLevel > indentLevel) {
			while (_indentLevel > indentLevel) {
				_append(TokenKind::indentPop);
				_indentLevel -= 1;
			}
		}
	}

	void _consumeComment() {
		unsigned int beginIndex = _contentIndex;

		while (_isBound(0) && _get(0) != '\n') {
			_advance(1);
		}

		_append(TokenKind::comment, beginIndex, _contentIndex - beginIndex);

		if (_isBound(0) && _get(0) == '\n') {
			_advance(1);
		}

	}

	void _consumeText() {
		unsigned int beginIndex = _contentIndex;

		while (_isBound(0)) {
			switch (_get(0)) {
				// fast early lookup
				case ' ':
				case ',':
				case '.':

				case '0': case '1': case '2': case '3': case '4':
				case '5': case '6': case '7': case '8': case '9':

				case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
				case 'H': case 'I': case 'J': case 'K': case 'L': case 'M':
				case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': 
				case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':

				case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
				case 'h': case 'i': case 'j': case 'k': case 'l': case 'm':
				case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': 
				case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':

				// slow case
				default:
					_advance(1);
					break;

				case '*':
				case '_':
				case '`':
				case '~':
				case '<':
				case '\n':
					goto end;

			}
		}

		end:

		_append(TokenKind::text, beginIndex, _contentIndex - beginIndex);

		return;
	}

	void _consumeIndex() {
		unsigned int beginIndex = _contentIndex;

		while (_isBound(0) && _get(0) >= '0' && _get(0) <= '9') {
			_advance(1);
		}

		if (_isBound(1) && _get(0) == '.' && _get(1) == ' ') {
			_advance(1);
			_append(TokenKind::index, beginIndex, _contentIndex - beginIndex);
			_advance(1);
			return;
		}

		_contentIndex = beginIndex;
		_consumeText();
	}

private:
	StringSlice _content;

	unsigned int _contentIndex;

	Array<Token> _tokens;

	unsigned int _indentLevel;
};

}
