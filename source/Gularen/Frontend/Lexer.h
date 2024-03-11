#pragma once

#include "Gularen/Library/Array.h"
#include "Gularen/Library/Slice.h"
#include "Gularen/Library/StringSlice.h"

namespace Gularen {

enum class TokenKind {
	comment,
	annotationKey,
	annotationValue,

	text,

	newline,
	newlinePlus,

	asterisk,
	underscore,
	backtick,

	head3,
	head2,
	head1,

	indentOpen,
	indentClose,

	blockquoteOpen,
	blockquoteClose,

	lineBreak,
	pageBreak,

	dinkus,

	bullet,
	index,
	checkbox,

	pipe,
	tee,
	teeLeft,
	teeRight,
	teeCenter,

	raw,

	fenceOpen,
	fenceClose,

	curlyOpen,
	curlyClose,

	parenOpen,
	parenClose,

	squareOpen,
	squareClose,

	angleOpen,
	angleClose,

	exclamation,
	question,
	caret,
	equal,

	hyphen,
	enDash,
	emDash,

	quoteOpen,
	quoteClose,

	squoteOpen,
	squoteClose,

	admon,
	dateTime,
	emoji,

	accountTag,
	hashTag,
};

StringSlice toStringSlice(TokenKind kind) {
	switch (kind) {
		case TokenKind::comment: return "comment";
		case TokenKind::annotationKey: return "annotationKey";
		case TokenKind::annotationValue: return "annotationValue";

		case TokenKind::text: return "text";

		case TokenKind::newline: return "newline";
		case TokenKind::newlinePlus: return "newline+";

		case TokenKind::asterisk: return "asterisk";
		case TokenKind::underscore: return "underscore";
		case TokenKind::backtick: return "backtick";

		case TokenKind::head3: return "head3";
		case TokenKind::head2: return "head2";
		case TokenKind::head1: return "head1";

		case TokenKind::indentOpen: return "indentOpen";
		case TokenKind::indentClose: return "indentClose";

		case TokenKind::blockquoteOpen: return "blockquoteOpen";
		case TokenKind::blockquoteClose: return "blockquoteClose";

		case TokenKind::lineBreak: return "lineBreak";
		case TokenKind::pageBreak: return "pageBreak";
		case TokenKind::dinkus: return "dinkus";

		case TokenKind::bullet: return "bullet";
		case TokenKind::index: return "index";
		case TokenKind::checkbox: return "checkbox";

		case TokenKind::pipe: return "pipe";
		case TokenKind::tee: return "tee";
		case TokenKind::teeLeft: return "teeLeft";
		case TokenKind::teeRight: return "teeRight";
		case TokenKind::teeCenter: return "teeCenter";

		case TokenKind::raw: return "raw";

		case TokenKind::fenceOpen: return "fenceOpen";
		case TokenKind::fenceClose: return "fenceClose";

		case TokenKind::curlyOpen: return "curlyOpen";
		case TokenKind::curlyClose: return "curlyClose";

		case TokenKind::parenOpen: return "parenOpen";
		case TokenKind::parenClose: return "parenClose";

		case TokenKind::squareOpen: return "squareOpen";
		case TokenKind::squareClose: return "squareClose";

		case TokenKind::angleOpen: return "angleOpen";
		case TokenKind::angleClose: return "angleClose";

		case TokenKind::exclamation: return "exclamation";
		case TokenKind::question: return "question";
		case TokenKind::caret: return "caret";
		case TokenKind::equal: return "equal";

		case TokenKind::hyphen: return "hyphen";
		case TokenKind::enDash: return "enDash";
		case TokenKind::emDash: return "emDash";

		case TokenKind::quoteOpen: return "quoteOpen";
		case TokenKind::quoteClose: return "quoteClose";

		case TokenKind::squoteOpen: return "squoteOpen";
		case TokenKind::squoteClose: return "squoteClose";

		case TokenKind::emoji: return "emoji";
		case TokenKind::dateTime: return "dateTime";
		case TokenKind::admon: return "admon";

		case TokenKind::accountTag: return "accountTag";
		case TokenKind::hashTag: return "hashTag";
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
		_quoteLevel = 0;

		_line = 0;
		_column = 0;

		_savePosition();

		_consumeIndent();
		_parseBlock();

		if (_tokens.size() != 0) {
			switch (_tokens.get(_tokens.size() - 1).kind) {
				case TokenKind::newlinePlus: 
					break;
				case TokenKind::newline: 
					_tokens.get(_tokens.size() - 1).kind = TokenKind::newlinePlus;
					break;
				default: 
					_append(TokenKind::newlinePlus);
					_consumeIndent();
					break;
			}
		}


		return Slice<Token>(_tokens.pointer(), _tokens.size());
	}

private:
	void _parseBlock() {
		while (_isBound(0)) {
			_savePosition();

			switch (_get(0)) {
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

					_consumeText();
					break;

				case '<':
					if (_isBound(1) && !(_get(1) >= '0' && _get(1) <= '9')) {
						unsigned int oldContentIndex = _contentIndex;

						while (_isBound(0) && _get(0) != '>') {
							_advance(1);
						}

						if (_isBound(0) && _get(0) == '>') {
							_append(TokenKind::admon, oldContentIndex + 1, _contentIndex - 1 - oldContentIndex);
							_advance(1);
							break;
						}

						_contentIndex = oldContentIndex;
					}

					_parseInline();
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

					_parseInline();
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

				case '?':
					if (_isBound(1) && _get(1) == '[') {
						_append(TokenKind::question, _contentIndex, 1);
						_advance(1);
						break;
					}

					_consumeText();
					break;

				case '=':
					if (_isBound(1) && _get(1) == '[') {
						_append(TokenKind::equal, _contentIndex, 1);
						_advance(1);
						break;
					}

					_consumeText();
					break;

				case '|':
					_consumePipe();
					break;

				default: 
					_parseInline(); 
					break;
			}
		}
	}

	void _parseInline() {
		while (_isBound(0)) {
			_savePosition();

			switch (_get(0)) {
				case '~':
					_consumeComment();
					return;

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

				case '<': {
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

					unsigned int oldContentIndex = _contentIndex;

					_advance(1);

					if (_get(0) >= '0' && _get(0) <= '9') {
						while (_isBound(0) && ((_get(0) >= '0' && _get(0) <= '9') || _get(0) == ':' || _get(0) == ' ' || _get(0) == '-')) {
							_advance(1);
						}

						if (_isBound(0) && _get(0) == '>') {
							_append(TokenKind::dateTime, oldContentIndex + 1, _contentIndex - oldContentIndex - 1);
							_advance(1);
							break;
						}
					}

					_append(TokenKind::text, oldContentIndex, _contentIndex - oldContentIndex);
					break;
				}

				case '-':
					if (_isBound(1) && _get(1) == '-') {
						if (_isBound(2) && _get(2) == '-') {
							_append(TokenKind::emDash, _contentIndex, 3);
							_advance(3);
							break;
						}

						_append(TokenKind::enDash, _contentIndex, 2);
						_advance(2);
						break;
					}

					_append(TokenKind::hyphen, _contentIndex, 1);
					_advance(1);
					break;

				case '[':
					if (_isBound(4) && _get(2) == ']' && _get(3) == ' ' && (_get(1) == ' ' || _get(1) == 'x' || _get(1) == 'v')) {
						_append(TokenKind::checkbox, _contentIndex, 3);
						_advance(4);
						break;
					}

					_consumeLink();
					break;

				case '!':
					if (_isBound(1) && _get(1) == '[') {
						_append(TokenKind::exclamation, _contentIndex, 1);
						_advance(1);
						break;
					}

					_consumeText();
					break;

				case '^':
					if (_isBound(1) && _get(1) == '[') {
						_append(TokenKind::caret, _contentIndex, 1);
						_advance(1);
						break;
					}

					_consumeText();
					break;

				case '|':
					_consumePipe();
					break;

				case '{':
					_consumeCode();
					break;

				case ':': {
					unsigned int openingContextIndex = _contentIndex;
					if (_isBound(1) && ((_get(1) >= 'a' && _get(1) <= 'z') || _get(1) == '-')) {
						_advance(1);
						while (_isBound(0) && ((_get(0) >= 'a' && _get(0) <= 'z') || _get(0) == '-')) {
							_advance(1);
						}
						if (_isBound(0) && _get(0) == ':') {
							_append(TokenKind::emoji, openingContextIndex + 1, _contentIndex - openingContextIndex - 1);
							_advance(1);
							break;
						}
					}
					_advance(1);
					_append(TokenKind::text, openingContextIndex, _contentIndex - openingContextIndex);
					break;
				}

				case '"':
					_consumeQuote(
						(_tokens.size() != 0 && _tokens.get(_tokens.size() - 1).kind == TokenKind::squoteOpen), // ‘“ case
						TokenKind::quoteOpen, 
						TokenKind::quoteClose
					);
					break;
					

				case '\'':
					_consumeQuote(
						(_tokens.size() != 0 && _tokens.get(_tokens.size() - 1).kind == TokenKind::quoteOpen), // “‘ case
						TokenKind::squoteOpen, 
						TokenKind::squoteClose
					);
					break;

				case '\n': {
					unsigned int count = 0;

					while (_isBound(0) && _get(0) == '\n') {
						_advanceLine(1);
						count += 1;
						_advance(1);
					}

					if (count == 1) {
						_append(TokenKind::newline);
						_consumeIndent();
						return;
					}

					_append(TokenKind::newlinePlus);
					_consumeIndent();
					return;
				}

				case '@': {
					unsigned int oldContentIndex = _contentIndex;
					_advance(1);

					while (_isBound(0) && (
						(_get(0) >= 'a' && _get(0) <= 'z') ||
						(_get(0) >= 'A' && _get(0) <= 'Z') ||
						(_get(0) >= '0' && _get(0) <= '9') ||
						(_get(0) == '_')
					)) {
						_advance(1);
					}

					_append(TokenKind::accountTag, oldContentIndex, _contentIndex - oldContentIndex);
					break;
				}

				case '#': {
					unsigned int oldContentIndex = _contentIndex;
					_advance(1);

					while (_isBound(0) && (
						(_get(0) >= 'a' && _get(0) <= 'z') ||
						(_get(0) >= 'A' && _get(0) <= 'Z') ||
						(_get(0) >= '0' && _get(0) <= '9') ||
						(_get(0) == '_')
					)) {
						_advance(1);
					}

					_append(TokenKind::hashTag, oldContentIndex, _contentIndex - oldContentIndex);
					break;
				}

				default: 
					_consumeText(); 
					break;
			}
		}
	}

private:
	bool _isBound(unsigned int offset) const {
		return _contentIndex + offset < _content.size();
	}

	void _advance(unsigned int offset) {
		_contentIndex += offset;
		_column += 1;
	}

	void _advanceLine(unsigned int offset) {
		_line += offset;
		_column = 0;
	}

	void _savePosition() {
		_oldLine = _line;
		_oldColumn = _column;
	}

	char _get(unsigned int offset) const {
		return _content.get(_contentIndex + offset);
	}

	void _append(TokenKind kind, unsigned int index = 0, unsigned int size = 0) {
		Token token;
		token.position.line = _oldLine;
		token.position.column = _oldColumn;
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
			return _consumeBlockquote();
		}

		if (_indentLevel < indentLevel) {
			while (_indentLevel < indentLevel) {
				_append(TokenKind::indentOpen);
				_indentLevel += 1;
			}
		}

		if (_indentLevel > indentLevel) {
			while (_indentLevel > indentLevel) {
				_append(TokenKind::indentClose);
				_indentLevel -= 1;
			}
		}

		_consumeBlockquote();
	}

	void _consumeBlockquote() {
		unsigned int quoteLevel = 0;
		while (_isBound(1) && _get(0) == '/' && _get(1) == ' ') {
			quoteLevel += 1;
			_advance(2);
		}

		if (_isBound(1) && _get(0) == '/' && _get(1) == '\n') {
			quoteLevel += 1;
			_advance(1);
		}

		if (_quoteLevel == quoteLevel) {
			return;
		}

		if (_quoteLevel < quoteLevel) {
			while (_quoteLevel < quoteLevel) {
				_append(TokenKind::blockquoteOpen);
				_quoteLevel += 1;
			}
		}

		if (_quoteLevel > quoteLevel) {
			while (_quoteLevel > quoteLevel) {
				_append(TokenKind::blockquoteClose);
				_quoteLevel -= 1;
			}
		}
	}

	void _consumeQuote(bool condition, TokenKind left, TokenKind right) {
		if (_contentIndex == 0 || 
			_content.get(_contentIndex - 1) == ' ' || 
            _content.get(_contentIndex - 1) == '\t' || 
            _content.get(_contentIndex - 1) == '\n' || 
            condition) {
			_append(left, _contentIndex, 1);
			_advance(1);
            return;
        }

		_append(right, _contentIndex, 1);
		_advance(1);
        return;
	}

	void _consumeComment() {
		unsigned int beginIndex = _contentIndex + 1;
		
		if (_isBound(2) && _get(1) == '~' && _get(2) == ' ') {
			_advance(3);
			unsigned int keyIndex = _contentIndex;

			while (_isBound(0) && (
				(_get(0) >= 'a' && _get(0) <= 'z') ||
				(_get(0) >= 'A' && _get(0) <= 'Z') ||
				(_get(0) >= '0' && _get(0) <= '9') ||
				(_get(0) == '-')
			)) {
				_advance(1);
			}

			if (_isBound(0) && _get(0) == ':') {
				_append(TokenKind::annotationKey, keyIndex, _contentIndex - keyIndex);
				_advance(1);
				while (_isBound(0) && _get(0) == ' ') {
					_advance(1);
				}
				unsigned int valueIndex = _contentIndex;

				while (_isBound(0) && _get(0) != '\n') {
					_advance(1);
				}

				_append(TokenKind::annotationValue, valueIndex, _contentIndex - valueIndex);

				if (_isBound(0) && _get(0) == '\n') {
					_advanceLine(1);
					_advance(1);
				}
				return;
			}
		}

		while (_isBound(0) && _get(0) != '\n') {
			_advanceLine(1);
			_advance(1);
		}

		_append(TokenKind::comment, beginIndex, _contentIndex - beginIndex);

		if (_isBound(0) && _get(0) == '\n') {
			_advanceLine(1);
			_advance(1);
		}
	}

	void _consumeText() {
		bool previousAlphanumeric = false;
		unsigned int beginIndex = _contentIndex;

		while (_isBound(0)) {
			switch (_get(0)) {
				// fast early lookup
				case ' ':
				case ',':
				case '.':
					previousAlphanumeric = false;
					_advance(1);
					break;

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
					previousAlphanumeric = true;
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
				case ':':
				case '-':
				case '"':
				case '\'':
				case '\n':
					goto end;

				case '@':
				case '#':
					if (!previousAlphanumeric) {
						goto end;
					}
					_advance(1);
					break;

				case '!':
				case '?':
				case '^':
				case '=':
					previousAlphanumeric = false;
					if (_isBound(1) && _get(1) == '[') {
						goto end;
					}

					_advance(1);
					break;
						
				// slow case
				default:
					previousAlphanumeric = false;
					_advance(1);
					break;
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
					_append(TokenKind::tee);
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
		_savePosition();

		unsigned int oldContextIndex = _contentIndex;

		while (_isBound(0) && _get(0) != ')') {
			_advance(1);
		}

		_append(TokenKind::raw, oldContextIndex, _contentIndex - oldContextIndex);
		_savePosition();

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
		_savePosition();

		unsigned int oldContextIndex = _contentIndex;

		while (_isBound(0) && _get(0) != '}') {
			_advance(1);
		}

		_append(TokenKind::raw, oldContextIndex, _contentIndex - oldContextIndex);
		_savePosition();

		if (_isBound(0) && _get(0) == '}') {
			_append(TokenKind::curlyClose, _contentIndex, 1);
			_advance(1);
			_savePosition();
		}

		if (_isBound(0) && _get(0) == '(') {
			_consumeLabel();
		}
	}

	void _consumeCodeBlockContent(unsigned int dashCount) {
		unsigned int oldContextIndex = _contentIndex;
		unsigned int oldIndentLevel = _indentLevel;

		while (_isBound(0)) {
			if (_isBound(0) && _get(0) == '\n') {
				_advanceLine(1);
				_advance(1);
				unsigned int indentLevel = 0;
				while (_isBound(0) && _get(0) == '\t') {
					_advance(1);
					indentLevel += 1;
				}

				if (_get(0) == '-' && indentLevel == oldIndentLevel) {
					unsigned int i = 0;
					while (_isBound(0) && i < dashCount && _get(i) == '-') { i += 1; }
					if (i == dashCount && (!_isBound(dashCount) || (_isBound(dashCount) && _get(dashCount) == '\n'))) {
						unsigned int size = _contentIndex - oldContextIndex - oldIndentLevel;
						if (size > 0) {
							size -= 1;

							if (size > 0) {
								size -= 1;
							}
						}
						_append(TokenKind::raw, oldContextIndex + 1, size);
						_append(TokenKind::fenceClose, _contentIndex, dashCount);
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
			_append(TokenKind::fenceOpen, oldContentIndex, _contentIndex - oldContentIndex);
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
				_append(TokenKind::fenceOpen, oldContentIndex, youngContextIndex - oldContentIndex);
				_append(TokenKind::text, middleAgedContentIndex, _contentIndex - middleAgedContentIndex);
				return _consumeCodeBlockContent(dashCount);
			}
		}

		_contentIndex = oldContentIndex;
		_parseInline();
	}

private:
	StringSlice _content;

	unsigned int _contentIndex;

	unsigned int _line;

	unsigned int _column;

	unsigned int _oldLine;

	unsigned int _oldColumn;

	Array<Token> _tokens;

	unsigned int _indentLevel;

	unsigned int _quoteLevel;
};

}
