#include "lexeme.hpp"
#include <stdio.h>

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
	bool _isTableLine;
	int _parenthesis;

	Lexer(String const& source) {
		_source = source;

		_point.index = 0;
		_point.position.line = 0;
		_point.position.column = 0;
		_indent = 0;
		_isHeadingLine = false;
		_isTableLine = false;
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
			case '`':
				_lexemeCode();
				break;
			case '|':
				_lexemeTable();
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
			return _appendInclusive(TokenKind_text, point, _point);
		}
		if (!_is(' ')) {
			return _appendInclusive(TokenKind_text, point, _point);
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
					_appendInclusive(TokenKind_thematicbreak, point, _point);
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
		_appendInclusive(TokenKind_text, point, _point);
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

	void _lexemeCode() {
		Point p = _point;
		int count = 0;
		while (_has()) {
			if (_get() == '`') {
				_advance();
				count++;
				continue;
			}
			break;
		}
		if (count == 1) {
			_point = p;
			_lexemeLineCode();
			return;
		}
		if (count >= 3) {
			if (_is(' ')) {
				_advance();
				Point langPoint = _point;
				while (_has()) {
					switch (_get()) {
						case 'a': case 'b': case 'c': case 'd':
						case 'e': case 'f': case 'g': case 'h':
						case 'i': case 'j': case 'k': case 'l':
						case 'm': case 'n': case 'o': case 'p':
						case 'q': case 'r': case 's': case 't':
						case 'u': case 'v': case 'w': case 'x':
						case 'y': case 'z': case '-':
						case '0': case '1': case '2': case '3':
						case '4': case '5': case '6': case '7':
						case '8': case '9':
							_advance();
							break;
						case '\n':
							goto endLanguage;
						default:
							goto fallback;
					}
				}
				endLanguage:
				_appendInclusive(TokenKind_openfence, p, langPoint);
				_appendInclusive(TokenKind_lang, langPoint, _point);
				_lexemeNewline();
				_lexemeSources(p.position.column, count);
				return;
			} else if (_is('\n')) {
				_appendInclusive(TokenKind_openfence, p, _point);
				_lexemeNewline();
				_lexemeSources(p.position.column, count);
				return;
			}
		}

		fallback:
		_appendInclusive(TokenKind_text, p, _point);
		return;
	}

	void _lexemeSources(int column, int startCount) {
		Point startSourcePoint = _point;
		Point endSourcePoint = _point;
		while (_has()) {
			int depth = 0;
			if (_get() == '\t') {
				while (_has()) {
					if (_get() == '\t') {
						depth++;
						_advance();
						continue;
					}
					break;
				}
			}
			if (depth == column && _is('`')) {
				Point fencePoint = _point;
				int endCount = 0;
				while (_has()) {
					if (_get() == '`') {
						endCount++;
						_advance();
						continue;
					}
					break;
				}
				if (_is('\n')) {
					if (startCount == endCount) {
						_appendInclusive(TokenKind_sources, startSourcePoint, endSourcePoint);
						_appendInclusive(TokenKind_closefence, fencePoint, _point);
						return;
					}
					endSourcePoint = _point;
					_point.position.line++;
					_point.position.column = 0;
					_point.index++;
					continue;
				}
			}
			while (_has()) {
				if (_get() == '\n') {
					endSourcePoint = _point;
					_point.position.line++;
					_point.position.column = 0;
					_point.index++;
					break;
				}
				_advance();
			}
		}
	}

	void _lexemeTable() {
		_isTableLine = true;

		while (_has()) {
			if (_get() == '|') {
				_lexemeMonograph(TokenKind_pipe);
				Point point = _point;

				if (_point.index + 2 < _source.size() &&
					(_source[_point.index + 0] == '-' || _source[_point.index + 0] == ':') &&
					(_source[_point.index + 1] == '-') && 
					(_source[_point.index + 2] == '-' || _source[_point.index + 2] == ':')) {
					_advance();

					while (_has()) {
						if (_get() == '-') {
							_advance();
							continue;
						}
						break;
					}
					if (_is(':')) {
						_advance();
					}
					if (_point.index < _source.size() && _source[_point.index] == '|') {
						_appendInclusive(TokenKind_bar, point, _point);
						continue;
					}
					_appendInclusive(TokenKind_text, point, _point);
					return;
				}
			}
			break;
		}
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
						_append(TokenKind_text, p, _point);
					}
					break;
				case ')':
					if (_parenthesis > 0) {
						_parenthesis--;
						return;
					} else {
						Point p = _point;
						_advance();
						_append(TokenKind_text, p, _point);
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
				case '|':
					if (_isTableLine) {
						_lexemeMonograph(TokenKind_pipe);
						break;
					}
					_lexemeMonograph(TokenKind_text);
					break;
				case '`':
					_lexemeLineCode();
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
					_append(TokenKind_text, p, _point);
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
						_appendInclusive(TokenKind_footnote, p, _point);
						p = _point;
						_advance();
						_lexemeLabel(p);
						break;
					}
					_append(TokenKind_text, p, _point);
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

	void _lexemeLineCode() {
		Point p = _point;
		_lexemeMonograph(TokenKind_backtick);
		Point sourcePoint = _point;
		if (_has()) {
			bool isValidLang = true;
			while (_has()) {
				if (_get() == '`') {
					Point checkPoint = _point;
					_advance();
					if (_is('`') && isValidLang) {
						_appendInclusive(TokenKind_lang, sourcePoint, checkPoint);
						_point = checkPoint;
						break;
					}
					_appendInclusive(TokenKind_source, sourcePoint, checkPoint);
					_point = checkPoint;
					break;
				}
				switch (_get()) {
					case 'a': case 'b': case 'c': case 'd':
					case 'e': case 'f': case 'g': case 'h':
					case 'i': case 'j': case 'k': case 'l':
					case 'm': case 'n': case 'o': case 'p':
					case 'q': case 'r': case 's': case 't':
					case 'u': case 'v': case 'w': case 'x':
					case 'y': case 'z': case '-':
					case '0': case '1': case '2': case '3':
					case '4': case '5': case '6': case '7':
					case '8': case '9':
						break;
					default:
						isValidLang = false;
						break;
				}
				_advance();
			}
			if (_is('`')) {
				_lexemeMonograph(TokenKind_backtick);
				return;
			}
		}
		_appendInclusive(TokenKind_text, p, _point);
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
					return _appendInclusive(TokenKind_text, point, _point);
			}
		}
		_appendInclusive(TokenKind_text, point, _point);
	}

	void _lexemeHyphen() {
		Point point = _point;
		_advance();
		if (_is('-')) {
			_advance();
			if (_is('-')) {
				_advance();
				_appendInclusive(TokenKind_emdash, point, _point);
				return;
			}
			_appendInclusive(TokenKind_endash, point, _point);
			return;
		}
		_appendInclusive(TokenKind_hyphen, point, _point);
		return;
	}

	void _lexemeResource(Point point, bool isLabeled = true) {
		Point endPoint = _point;

		// parse quoted 
		if (_is('"')) {
			Point refPoint = _point;
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
			_appendInclusive(TokenKind_openref, point, endPoint);
			_appendInclusive(TokenKind_quotedref, refPoint, _point);
		} else {
			Point refPoint = _point;
			while (_has()) {
				if (_get() == ']') {
					break;
				}
				_advance();
			}

			if (_point.index + 1 < _source.size() && 
				_source[_point.index] == ']' &&
				(_source[_point.index + 1] == ' ' || _source[_point.index + 1] == '\n')) {
				String content = _source.slice(refPoint.index, _point.index - refPoint.index);
				if (content == "NOTE" ||
					content == "HINT" ||
					content == "IMPORTANT" ||
					content == "WARNING" ||
					content == "SEE" ||
					content == "TIP") {
					_advance();
					if (_is(' ')) {
						_advance();
					}
					_appendInclusive(TokenKind_admon, point, _point);
					return;
				}
			}

			_appendInclusive(TokenKind_openref, point, endPoint);
			_appendInclusive(TokenKind_ref, refPoint, _point);
		}
		if (_is(']')) {
			point = _point;
			_advance();
			_appendInclusive(TokenKind_closeref, point, _point);

			if (isLabeled && _is('(')) {
				point = _point;
				_advance();
				_lexemeLabel(point);
			}
		}
	}

	void _lexemeLabel(Point point) {
		_appendInclusive(TokenKind_openlabel, point, _point);
		_parenthesis = 1;
		while (_has()) {
			if (_get() == ')') {
				if (_parenthesis == 0) {
					point = _point;
					_advance();
					_appendInclusive(TokenKind_closelabel, point, _point);
					break;
				} else {
					point = _point;
					_advance();
					_appendInclusive(TokenKind_text, point, _point);
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
			_appendInclusive(leftKind, point, _point);
			return;
		}
		_advance();
		_appendInclusive(rightKind, point, _point);
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
		_appendInclusive(TokenKind_hashtag, point, _point);
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
		_appendInclusive(TokenKind_text, startPoint, _point);
	}

	void _lexemeEmoji() {
		Point startPoint = _point;
		_advance();

		if (_is(' ')) {
			if (_isHeadingLine) {
				_advance();
				_appendInclusive(TokenKind_subheading, startPoint, _point);
				return;
			}
			_advance();
			_appendInclusive(TokenKind_text, startPoint, _point);
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
		_appendInclusive(TokenKind_emoji, startPoint, _point);
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
			_advance();
			_appendInclusive(TokenKind_escape, point, _point);
		}
	}

	void _lexemeNewline() {
		Point startPoint = _point;
		Point endPoint = _point;
		int count = 0;
		while (_is('\n')) {
			endPoint = _point;
			count++;
			_point.index++;
			_point.position.line++;
			_point.position.column = 0;
		}
		_isHeadingLine = false;
		_isTableLine = false;
		_append(count > 1 ? TokenKind_newlines : TokenKind_newline, startPoint, endPoint);
	}

	void _append(TokenKind kind, Point start, Point end) {
		Token token;
		token.kind = kind;
		token.content = _source.slice(start.index, end.index - start.index + 1);
		token.range.start = start.position;
		token.range.end = end.position;
		_tokens.append(token);
	}

	void _appendInclusive(TokenKind kind, Point start, Point end) {
		Token token;
		token.kind = kind;
		token.content = _source.slice(start.index, end.index - start.index);
		token.range.start = start.position;
		token.range.end = end.position;
		token.range.end.column--;
		_tokens.append(token);
	}
};

Array<Token> lexeme(String const& source) {
	Lexer lexer(source);
	return lexer.lexeme();
}

