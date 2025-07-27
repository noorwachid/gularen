#include "lexeme.hpp"

struct Point {
	int index;
	Position position;
};

struct Lexer {
	Point _point;
	String _source;
	Array<Token> _tokens;

	Lexer(String const& source) {
		_source = source;

		_point.index = 0;
		_point.position.line = 0;
		_point.position.column = 0;
	}

	Array<Token> lexeme() {
		while (_has()) {
			_lexemeBlock();
		}
		return _tokens;
	}

	bool _has() {
		return _point.index < _source.size();
	}

	bool _is(Byte byte) {
		return _point.index < _source.size() && _source[_point.index] == byte;
	}

	Byte _get() {
		return _source[_point.index];
	}

	void _advance() {
		_point.index++;
		_point.position.column++;
	}

	void _lexemeBlock() {
		switch (_get()) {
			case '>':
				_lexemeHeading();
				break;
			default:
				_lexemeLine();
				break;
		}
	}

	void _lexemeHeading() {
		Point point = _point;
		int count = 0;
		while (_has()) {
			if (_get() == '>') {
				count++;
				_advance();
				continue;
			}
			break;
		}
		if (count > 3) {
			return _appendText(point, _point);
		}
		if (!_is(' ')) {
			return _appendText(point, _point);
		}
		_append(TokenKind_heading, point, _point);
	}

	void _lexemeLine() {
		switch (_get()) {
			case 'a': case 'b': case 'c': case 'd':
			case 'e': case 'f': case 'g': case 'h':
			case 'i': case 'j': case 'k': case 'l':
			case 'm': case 'n': case 'o': case 'p':
			case 'q': case 'r': case 's': case 't':
			case 'u': case 'v': case 'w': case 'x':
			case 'y': case 'z': case ' ':
			case 'A': case 'B': case 'C': case 'D':
			case 'E': case 'F': case 'G': case 'H':
			case 'I': case 'J': case 'K': case 'L':
			case 'M': case 'N': case 'O': case 'P':
			case 'Q': case 'R': case 'S': case 'T':
			case 'U': case 'V': case 'W': case 'X':
			case 'Y': case 'Z': case '.': case ',':
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
			case '8': case '9':
				_lexemeText();
				break;

			case '*':
				_lexemeMonograph(TokenKind_asterisk);
				break;

			case '_':
				_lexemeMonograph(TokenKind_underscore);
				break;

			case '\n':
				_lexemeNewline();
				break;

			case '#':
				_lexemeHash();
				break;

			default:
				_advance();
				break;
		}
	}

	void _lexemeHash() {
		Point point = _point;
		_advance();

		if (_has()) {
			switch (_get()) {
				case ' ':
					return _lexemeComment();

				case 'a': case 'b': case 'c': case 'd':
				case 'e': case 'f': case 'g': case 'h':
				case 'i': case 'j': case 'k': case 'l':
				case 'm': case 'n': case 'o': case 'p':
				case 'q': case 'r': case 's': case 't':
				case 'u': case 'v': case 'w': case 'x':
				case 'y': case 'z': case '_':
				case 'A': case 'B': case 'C': case 'D':
				case 'E': case 'F': case 'G': case 'H':
				case 'I': case 'J': case 'K': case 'L':
				case 'M': case 'N': case 'O': case 'P':
				case 'Q': case 'R': case 'S': case 'T':
				case 'U': case 'V': case 'W': case 'X':
				case 'Y': case 'Z':
					return _lexemeHashtag(point);

				default:
					return _appendText(point, _point);
			}
		}
		_appendText(point, _point);
	}

	void _lexemeComment() {
		_advance();
		while (_has()) {
			if (_get() == '\n') {
				return;
			}
			_advance();
		}
	}

	void _lexemeHashtag(Point point) {
		while (_has()) {
			switch (_get()) {
				case 'a': case 'b': case 'c': case 'd':
				case 'e': case 'f': case 'g': case 'h':
				case 'i': case 'j': case 'k': case 'l':
				case 'm': case 'n': case 'o': case 'p':
				case 'q': case 'r': case 's': case 't':
				case 'u': case 'v': case 'w': case 'x':
				case 'y': case 'z': case '_':
				case 'A': case 'B': case 'C': case 'D':
				case 'E': case 'F': case 'G': case 'H':
				case 'I': case 'J': case 'K': case 'L':
				case 'M': case 'N': case 'O': case 'P':
				case 'Q': case 'R': case 'S': case 'T':
				case 'U': case 'V': case 'W': case 'X':
				case 'Y': case 'Z': case '.': case ',':
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
				case '8': case '9':
					_advance();
					break;
				default:
					goto end;
			}
		}

		end:
		Point endPoint = _point;
		endPoint.position.column -= 1;
		_append(TokenKind_hashtag, point, endPoint);
	}

	void _lexemeText() {
		Point startPoint = _point;
		while (_has()) {
			switch (_get()) {
				case 'a': case 'b': case 'c': case 'd':
				case 'e': case 'f': case 'g': case 'h':
				case 'i': case 'j': case 'k': case 'l':
				case 'm': case 'n': case 'o': case 'p':
				case 'q': case 'r': case 's': case 't':
				case 'u': case 'v': case 'w': case 'x':
				case 'y': case 'z': case ' ':
				case 'A': case 'B': case 'C': case 'D':
				case 'E': case 'F': case 'G': case 'H':
				case 'I': case 'J': case 'K': case 'L':
				case 'M': case 'N': case 'O': case 'P':
				case 'Q': case 'R': case 'S': case 'T':
				case 'U': case 'V': case 'W': case 'X':
				case 'Y': case 'Z': case '.': case ',':
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
				case '8': case '9':
					_advance();
					break;
				default:
					goto end;
			}
		}

		end:
		Point endPoint = _point;
		endPoint.position.column -= 1;
		_append(TokenKind_text, startPoint, endPoint);
	}

	void _lexemeMonograph(TokenKind kind) {
		Token token;
		token.kind = kind;
		token.content = _source.slice(_point.index, 1);
		token.range.start = _point.position;
		token.range.end = _point.position;
		_tokens.append(token);
		_advance();
	}

	void _lexemeNewline() {
		Point point = _point;
		int count = 0;
		while (_is('\n')) {
			count++;
			_point.index++;
			_point.position.line++;
			_point.position.column = 0;
		}
		_append(count > 1 ? TokenKind_newlinePlus : TokenKind_newline, point, point);
	}

	void _append(TokenKind kind, Point start, Point end) {
		Token token;
		token.kind = kind;
		token.content = _source.slice(start.index, end.index - start.index);
		token.range.start = start.position;
		token.range.end = end.position;
		_tokens.append(token);
	}

	void _appendText(Point start, Point end) {
		Token token;
		token.kind = TokenKind_text;
		token.content = _source.slice(start.index, end.index - start.index);
		token.range.start = start.position;
		token.range.end = end.position;
		_tokens.append(token);
	}
};

Array<Token> lexeme(String const& source) {
	Lexer lexer(source);
	return lexer.lexeme();
}

