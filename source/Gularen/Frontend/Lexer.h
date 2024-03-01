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

	pipe,
	teeLeft,
	teeRight,
	teeCenter,

	raw,

	fencePush,
	fencePop,

	curlyOpen,
	curlyClose,

	parenOpen,
	parenClose,

	squareOpen,
	squareClose,

	angleOpen,
	angleClose,
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

		case TokenKind::pipe: return "pipe";
		case TokenKind::teeLeft: return "teeLeft";
		case TokenKind::teeRight: return "teeRight";
		case TokenKind::teeCenter: return "teeCenter";

		case TokenKind::raw: return "raw";

		case TokenKind::fencePush: return "fencePush";
		case TokenKind::fencePop: return "fencePop";

		case TokenKind::curlyOpen: return "curlyOpen";
		case TokenKind::curlyClose: return "curlyClose";

		case TokenKind::parenOpen: return "parenOpen";
		case TokenKind::parenClose: return "parenClose";

		case TokenKind::squareOpen: return "squareOpen";
		case TokenKind::squareClose: return "squareClose";

		case TokenKind::angleOpen: return "angleOpen";
		case TokenKind::angleClose: return "angleClose";
	}
}

struct Position {
	unsigned int line;
	unsigned int column;
};

struct Token {
	Position position;
	TokenKind kind;
	StringSlice content;

	void print() const {
		StringSlice kindString = toStringSlice(kind);

		printf("%.*s", kindString.size(), kindString.pointer());

		if (content.size() != 0) {
			printf(" = ");
			for (unsigned int i = 0; i < content.size(); i += 1) {
				if (content.get(i) < ' ') {
					switch (content.get(i)) {
						case '\t':
							printf("\\t");
							break;

						case '\n':
							printf("\\n");
							break;

						default:
							printf("\\x%02X", content.get(i) & 0xFF);
							break;
					}
					continue;
				}
				printf("%c", content.get(i));
			}
		}

		printf("\n");
	}
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

					if (_isBound(2) && _get(1) == '-' && _get(2) == '-') {
						_consumeCodeBlock();
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

				case '[':
					if (_isBound(4) && _get(2) == ']' && _get(3) == ' ' && (_get(1) == ' ' || _get(1) == 'x' || _get(1) == 'v')) {
						_append(TokenKind::checkbox, _contentIndex, 3);
						_advance(4);
						break;
					}

					_consumeLink();
					break;

				case '|':
					_consumePipe();
					break;

				case '{':
					_consumeCode();
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
		token.content = _content.cut(index, size);
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
				case '|':
				case '{':
				case '[':
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

	void _consumePipe() {
		while (_isBound(0)) {
			switch (_get(0)) {
				case '|':
					_append(TokenKind::pipe);
					_advance(1);
					break;

				case '-':
					if (!(_isBound(2) && _get(1) == '-' && (_get(2) == '-' || _get(2) == ':'))) {
						_consumeText();
						break;
					}

					while (_isBound(0) && _get(0) == '-') {
						_advance(1);
					}

					if (_isBound(0) && _get(0) == ':') {
						_append(TokenKind::teeRight);
						_advance(1);
						break;
					}
					_append(TokenKind::teeLeft);
					break;

				case ':':
					if (!(_isBound(2) && _get(1) == '-' && (_get(2) == '-' || _get(2) == ':'))) {
						_consumeText();
						break;
					}

					_advance(1);

					while (_isBound(0) && _get(0) == '-') {
						_advance(1);
					}

					if (_isBound(0) && _get(0) == ':') {
						_append(TokenKind::teeCenter);
						_advance(1);
						break;
					}
					_append(TokenKind::teeLeft);
					break;

				case '\n':
					return;

				default:
					_consumeText();
					break;
			}
		}
	}

	void _consumeLabel() {
		_append(TokenKind::parenOpen, _contentIndex, 1);
		_advance(1);

		unsigned int oldContextIndex = _contentIndex;

		while (_isBound(0) && _get(0) != ')') {
			_advance(1);
		}

		_append(TokenKind::raw, oldContextIndex, _contentIndex - oldContextIndex);

		if (_isBound(0) && _get(0) == ')') {
			_append(TokenKind::parenClose, _contentIndex, 1);
			_advance(1);
		}
	}

	void _consumeLink() {
		_append(TokenKind::squareOpen, _contentIndex, 1);
		_advance(1);

		unsigned int oldContextIndex = _contentIndex;

		while (_isBound(0) && _get(0) != ']') {
			_advance(1);
		}

		_append(TokenKind::raw, oldContextIndex, _contentIndex - oldContextIndex);

		if (_isBound(0) && _get(0) == ']') {
			_append(TokenKind::squareClose, _contentIndex, 1);
			_advance(1);
		}

		if (_isBound(0) && _get(0) == '(') {
			_consumeLabel();
		}
	}

	void _consumeCode() {
		_append(TokenKind::curlyOpen, _contentIndex, 1);
		_advance(1);

		unsigned int oldContextIndex = _contentIndex;

		while (_isBound(0) && _get(0) != '}') {
			_advance(1);
		}

		_append(TokenKind::raw, oldContextIndex, _contentIndex - oldContextIndex);

		if (_isBound(0) && _get(0) == '}') {
			_append(TokenKind::curlyClose, _contentIndex, 1);
			_advance(1);
		}

		if (_isBound(0) && _get(0) == '(') {
			_consumeLabel();
		}
	}

	void _consumeCodeBlockContent(unsigned int dashCount) {
		unsigned int oldContextIndex = _contentIndex;
		unsigned int oldIndentLevel = _indentLevel;

		while (_isBound(0)) {
			if (_get(0) == '\n') {
				_advance(1);
				unsigned int indentLevel = 0;
				while (_isBound(0) && _get(0) == '\t') {
					_advance(1);
					indentLevel += 1;
				}

				if (_get(0) == '-' && indentLevel == oldIndentLevel) {
					unsigned int i = 0;
					while (i < dashCount && _get(i) == '-') { i += 1; }
					if (i == dashCount && (!_isBound(dashCount) || (_isBound(dashCount) && _get(dashCount) == '\n'))) {
						_append(TokenKind::raw, oldContextIndex + 1, _contentIndex - oldContextIndex - 2 - oldIndentLevel);
						_append(TokenKind::fencePop, _contentIndex, dashCount);
						_advance(dashCount);
						return;
					}
				}
				continue;
			}

			_advance(1);
		}

		_append(TokenKind::raw, oldContextIndex + 1, _contentIndex - oldContextIndex - 1);
	}

	void _consumeCodeBlock() {
		unsigned int oldContentIndex = _contentIndex;

		while (_isBound(0) && _get(0) == '-') {
			_advance(1);
		}

		unsigned int dashCount = _contentIndex - oldContentIndex;

		if (_isBound(0) && _get(0) == '\n') {
			_append(TokenKind::fencePush, oldContentIndex, _contentIndex - oldContentIndex);
			return _consumeCodeBlockContent(dashCount);
		}

		// capture label
		if (_isBound(1) && _get(0) == ' ' && _get(1) != '\n') {
			unsigned int youngContextIndex = _contentIndex;
			_advance(1);
			unsigned int middleAgedContentIndex = _contentIndex;

			while (_isBound(0) && _get(0) != '\n') {
				_advance(1);
			}

			if (_isBound(0) && _get(0) == '\n') {
				_append(TokenKind::fencePush, oldContentIndex, youngContextIndex - oldContentIndex);
				_append(TokenKind::text, middleAgedContentIndex, _contentIndex - middleAgedContentIndex);
				return _consumeCodeBlockContent(dashCount);
			}
		}

		_contentIndex = oldContentIndex;
		_consumeText();
	}

private:
	StringSlice _content;

	unsigned int _contentIndex;

	Array<Token> _tokens;

	unsigned int _indentLevel;
};

}
