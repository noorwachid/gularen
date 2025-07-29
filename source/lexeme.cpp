#include "lexeme.hpp"

struct Point {
	int index;
	Position position;
};

struct Lexer {
	Point _point;
	String _source;
	Array<Token> _tokens;
	int _indent;
	bool _isHeadingLine;
	int _parenthesis;

	Lexer(String const& source) {
		_source = source;

		_point.index = 0;
		_point.position.line = 0;
		_point.position.column = 0;
		_indent = 0;
		_isHeadingLine = false;
		_parenthesis = 0;
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
		_lexemeIndent();

		switch (_get()) {
			case '>':
				_lexemeHeading();
				break;
			case '-':
				_lexemeBullet();
				break;
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
			case '8': case '9': 
				_lexemeNumberPoint();
				break;
			case '[':
				_lexemeCheckbox();
				break;

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
			case ':': case '*': case '_': case '#':
			case '<':
			case '"':
			case '(': case ')': case '!': case '^':
			case '\'':
			case '\\':
			case '\n': 
				_lexemeLine();
				break;
			default:
				_advance();
				break;
		}
	}

	void _lexemeIndent() {
		Point point = _point;
		int count = 0;
		while (_has()) {
			if (_get() == '\t') {
				count++;
				_advance();
				continue;
			}
			break;
		}
		if (count > _indent) {
			for (int i = 0; i < count - _indent; i++) {
				_appendShadow(TokenKind_indent, point);
			}
		} else if (count < _indent) {
			for (int i = 0; i < _indent - count; i++) {
				_appendShadow(TokenKind_outdent, point);
			}
		}
		_indent = count;
	}

	void _lexemeHeading() {
		_isHeadingLine = true;
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
		_advance();
		_lexemeLine();
	}

	void _lexemeBullet() {
		Point point = _point;
		_advance();
		if (_is(' ')) {
			_append(TokenKind_bullet, point, _point);
			_advance();
			_lexemeLine();
			return;
		}
		if (_is('-')) {
			_advance();
			if (_is('-')) {
				_advance();
				while (_has()) {
					if (_get() != '-') {
						break;
					}
					_advance();
				}
				if (_is('\n')) {
					_appendSlice(TokenKind_thematicbreak, point, _point.index - point.index);
					return;
				}
			}
		}
		_point = point;
		_lexemeHyphen();
	}

	void _lexemeNumberPoint() {
		Point point = _point;
		_advance();
		while (_has()) {
			switch (_get()) {
				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
				case '8': case '9':
					continue;
				default:
					goto pointCheck;
			}
		}
		pointCheck:
		if (_is('.')) {
			_advance();
			if (_is(' ')) {
				_append(TokenKind_numberpoint, point, _point);
				_advance();
				_lexemeLine();
				return;
			}
		}
		_appendText(point, _point);
	}

	void _lexemeCheckbox() {
		Point point = _point;
		_advance();
		if (_has() && (_get() == ' ' || _get() == 'x')) {
			_advance();
			if (_is(']')) {
				_advance();
				if (_is(' ')) {
					_append(TokenKind_checkbox, point, _point);
					_advance();
					_lexemeLine();
					return;
				}
			}
		}
		_point = point;
		_lexemeLine();
	}

	void _lexemeLine() {
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
				case '>':
					_lexemeText();
					break;
				case '(': 
					if (_parenthesis > 0) {
						_parenthesis++;
						_advance();
					} else {
						Point p = _point;
						_advance();
						_appendSlice(TokenKind_text, p, _point.index - p.index);
					}
					break;
				case ')':
					if (_parenthesis > 0) {
						_parenthesis--;
						return;
					} else {
						Point p = _point;
						_advance();
						_appendSlice(TokenKind_text, p, _point.index - p.index);
					}
					break;
				case ':':
					_lexemeEmoji();
					break;
				case '*':
					_lexemeMonograph(TokenKind_asterisk);
					break;
				case '_':
					_lexemeMonograph(TokenKind_underscore);
					break;
				case '<':
					_lexemeMonograph(TokenKind_linebreak);
					break;
				case '#':
					_lexemeHash();
					break;
				case '-':
					_lexemeHyphen();
					break;
				case '!': {
					Point p = _point;
					_advance();
					if (_is('[')) {
						_advance();
						_lexemeResource(p);
						break;
					}
					_appendSlice(TokenKind_text, p, _point.index - p.index);
					break;
				}
				case '^': {
					Point p = _point;
					_advance();
					if (_is('[')) {
						_advance();
						_lexemeResource(p, false);
						break;
					}
					if (_is('(')) {
						_appendSlice(TokenKind_footnote, p, _point.index - p.index);
						p = _point;
						_advance();
						_lexemeLabel(p);
						break;
					}
					_appendSlice(TokenKind_text, p, _point.index - p.index);
					break;
				}
				case '[': {
					Point p = _point;
					_advance();
					_lexemeResource(p);
					break;
				}
				case '"':
					_lexemeQuote(
						_tokens.size() != 0 && _tokens[_tokens.size() - 1].kind == TokenKind_singleleftquote,
						TokenKind_leftquote,
						TokenKind_rightquote
					);
					break;
				case '\'':
					_lexemeQuote(
						_tokens.size() != 0 && _tokens[_tokens.size() - 1].kind == TokenKind_leftquote,
						TokenKind_singleleftquote,
						TokenKind_singlerightquote
					);
					break;
				case '\\':
					_lexemeEscape();
					break;
				case '\n':
					_lexemeNewline();
					return;
				default:
					return;
			}
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

	void _lexemeHyphen() {
		Point point = _point;
		_advance();
		if (_is('-')) {
			_advance();
			if (_is('-')) {
				_advance();
				_appendSlice(TokenKind_emdash, point, _point.index - point.index);
				return;
			}
			_appendSlice(TokenKind_endash, point, _point.index - point.index);
			return;
		}
		_appendSlice(TokenKind_hyphen, point, _point.index - point.index);
		return;
	}

	void _lexemeResource(Point point, bool isLabeled = true) {
		_appendSlice(TokenKind_openref, point, _point.index - point.index);

		// parse quoted 
		if (_is('"')) {
			point = _point;
			_advance();
			while (_has()) {
				switch (_get()) {
					case '"':
						_advance();
						goto endQuote;
					case '\\':
						_advance();
						_advance();
						break;
					default:
						_advance();
						break;
				}
			}
			endQuote:
			_appendSlice(TokenKind_quotedref, point, _point.index - point.index);
		} else {
			point = _point;
			while (_has()) {
				if (_get() == ']') {
					break;
				}
				_advance();
			}
			_appendSlice(TokenKind_ref, point, _point.index - point.index);
		}
		if (_is(']')) {
			point = _point;
			_advance();
			_appendSlice(TokenKind_closeref, point, _point.index - point.index);

			if (isLabeled && _is('(')) {
				point = _point;
				_advance();
				_lexemeLabel(point);
			}
		}
	}

	void _lexemeLabel(Point point) {
		_appendSlice(TokenKind_openlabel, point, _point.index - point.index);
		_parenthesis = 1;
		while (_has()) {
			if (_get() == ')') {
				if (_parenthesis == 0) {
					point = _point;
					_advance();
					_appendSlice(TokenKind_closelabel, point, _point.index - point.index);
					break;
				} else {
					point = _point;
					_advance();
					_appendSlice(TokenKind_text, point, _point.index - point.index);
				}
			}
			_lexemeLine();
		}
	}

	void _lexemeQuote(bool isNestedCombination /* ‘“ or “‘ */, TokenKind leftKind, TokenKind rightKind) {
		Point point = _point;
		if (_point.index == 0 || 
			_source[_point.index - 1] == ' ' ||
			_source[_point.index - 1] == '\t' ||
			_source[_point.index - 1] == '\n' ||
			isNestedCombination) {
			_advance();
			_appendSlice(leftKind, point, _point.index - point.index);
			return;
		}
		_advance();
		_appendSlice(rightKind, point, _point.index - point.index);
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
				case '>':
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

	void _lexemeEmoji() {
		Point startPoint = _point;
		_advance();

		if (_is(' ')) {
			if (_isHeadingLine) {
				_append(TokenKind_subheading, startPoint, _point);
				_advance();
				return;
			}
			_advance();
			_appendText(startPoint, _point);
			return;
		}

		while (_has()) {
			switch (_get()) {
				case 'a': case 'b': case 'c': case 'd':
				case 'e': case 'f': case 'g': case 'h':
				case 'i': case 'j': case 'k': case 'l':
				case 'm': case 'n': case 'o': case 'p':
				case 'q': case 'r': case 's': case 't':
				case 'u': case 'v': case 'w': case 'x':
				case 'y': case 'z': case '-':
					_advance();
					break;
				case ':':
					_advance();
					goto end;
				default:
					goto end;
			}
		}

		end:
		Point endPoint = _point;
		endPoint.position.column -= 1;
		_append(TokenKind_emoji, startPoint, endPoint);
	}

	void _appendShadow(TokenKind kind, Point point) {
		Token token;
		token.kind = kind;
		token.range.start = point.position;
		token.range.end = point.position;
		_tokens.append(token);
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

	void _lexemeEscape() {
		Point point = _point;
		_advance();
		if (_has()) {
			Token token;
			token.kind = TokenKind_text;
			token.content = _source.slice(point.index + 1, 1);
			token.range.start = point.position;
			token.range.end = _point.position;
			_advance();
			_tokens.append(token);
		}
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
		_isHeadingLine = false;
		_appendShadow(count > 1 ? TokenKind_newlines : TokenKind_newline, point);
	}

	void _appendSlice(TokenKind kind, Point start, int size) {
		Token token;
		token.kind = kind;
		token.content = _source.slice(start.index, size);
		token.range.start = start.position;
		token.range.end = start.position;
		token.range.end.column += size - 1;
		_tokens.append(token);
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

