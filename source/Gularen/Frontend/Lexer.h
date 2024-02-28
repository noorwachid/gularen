#pragma once

#include "Gularen/Library/Array.h"
#include "Gularen/Library/Slice.h"
#include "Gularen/Library/StringSlice.h"

namespace Gularen {

enum class TokenKind {
	text,

	asterisk,
	underscore,
	backtick,
};

StringSlice toStringSlice(TokenKind kind) {
	switch (kind) {
		case TokenKind::text: return "text";
		case TokenKind::asterisk: return "asterisk";
		case TokenKind::underscore: return "underscore";
		case TokenKind::backtick: return "backtick";
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

		while (_isBound(0)) {
			switch (_get(0)) {
				case '*': 
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

				default: 
					_consumeText(); 
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

	void _appendText(unsigned int index, unsigned int size) {
		_append(TokenKind::text, index, size);
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
					goto end;

			}
		}

		end:

		_appendText(beginIndex, _contentIndex - beginIndex);

		return;
	}

private:
	StringSlice _content;

	unsigned int _contentIndex;

	Array<Token> _tokens;
};

}
