#pragma once

#include <iostream>
#include <vector>
#include <string_view>

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

	equal,

	colon,
	coloncolon,

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

	parenOpen,
	parenClose,

	squareOpen,
	squareClose,

	angleOpen,
	angleClose,

	exclamation,
	question,
	caret,

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

std::string_view toStringView(TokenKind kind) {
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

		case TokenKind::equal: return "equal";

		case TokenKind::colon: return "colon";
		case TokenKind::coloncolon: return "coloncolon";

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

		case TokenKind::parenOpen: return "parenOpen";
		case TokenKind::parenClose: return "parenClose";

		case TokenKind::squareOpen: return "squareOpen";
		case TokenKind::squareClose: return "squareClose";

		case TokenKind::angleOpen: return "angleOpen";
		case TokenKind::angleClose: return "angleClose";

		case TokenKind::exclamation: return "exclamation";
		case TokenKind::question: return "question";
		case TokenKind::caret: return "caret";

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

struct Range {
	size_t startLine;
	size_t startColumn;
	size_t endLine;
	size_t endColumn;
};

struct Token {
	Range range;
	TokenKind kind;
	std::string_view content;

	void print() const {
		std::cout << toStringView(kind);

		if (content.size() != 0) {
			std::cout << " = ";
			for (size_t i = 0; i < content.size(); i += 1) {
				if (content[i] < ' ') {
					switch (content[i]) {
						case '\t':
							std::cout << "\\t";
							break;

						case '\n':
							std::cout << "\\n";
							break;

						default:
							std::cout << "\\x" << static_cast<int>(content[i]);
							break;
					}
					continue;
				}
				std::cout << content[i];
			}
		}

		printf("\n");
	}
};

class Lexer {
public:
	void parse(std::string_view content) {
		_content = content;
		_contentIndex = 0;
		_indentLevel = 0;
		_quoteLevel = 0;

		_line = 0;
		_column = 0;

		_saveRangeStart();

		_consumeIndent();
		_parseBlock();

		if (_tokens.size() != 0) {
			switch (_tokens[_tokens.size() - 1].kind) {
				case TokenKind::newlinePlus: 
					break;
				case TokenKind::newline: 
					_tokens[_tokens.size() - 1].kind = TokenKind::newlinePlus;
					break;
				default: 
					_append(TokenKind::newlinePlus);
					_consumeIndent();
					break;
			}
		}
	}

	const Token& operator[](size_t index) const {
		return _tokens[index];
	}

	inline size_t size() const {
		return _tokens.size();
	}

private:
	void _parseBlock() {
		while (_isBound(0)) {
			_saveRangeStart();

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
							break;
						}
					}

					if (_isBound(1) && _get(1) == ' ') {
						_append(TokenKind::head1, _contentIndex, 1);
						_advance(2);
						break;
					}

					_consumeText();
					break;

				case '<':
					if (_isBound(1) && !(_get(1) >= '0' && _get(1) <= '9') && _get(1) != '<') {
						size_t oldContentIndex = _contentIndex;

						while (_isBound(0) && _get(0) != '>') {
							_advance(1);
						}

						if (_isBound(0) && _get(0) == '>') {
							_append(TokenKind::admon, oldContentIndex + 1, _contentIndex - 1 - oldContentIndex, Range { _oldLine, _oldColumn, _line, _column });
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
			_saveRangeStart();

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
					_consumeCode();
					break;

				case '=': 
					_append(TokenKind::equal); 
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

					size_t oldContentIndex = _contentIndex;

					_advance(1);

					if (_get(0) >= '0' && _get(0) <= '9') {
						while (_isBound(0) && ((_get(0) >= '0' && _get(0) <= '9') || _get(0) == ':' || _get(0) == ' ' || _get(0) == '-')) {
							_advance(1);
						}

						if (_isBound(0) && _get(0) == '>') {
							_append(TokenKind::dateTime, oldContentIndex + 1, _contentIndex - 1 - oldContentIndex, Range { _oldLine, _oldColumn, _line, _column });
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
					if (_isBound(4) && _get(2) == ']' && _get(3) == ' ' && (_get(1) == ' ' || _get(1) == 'x')) {
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

					if (_isBound(1) && _get(1) == '(') {
						_append(TokenKind::caret, _contentIndex, 1);
						_advance(1);
						_consumeLabel();
						break;
					}

					_consumeText();
					break;

				case '|':
					_consumePipe();
					break;

				case ':': {
					if (_isBound(1) && _get(1) == ':') {
						_append(TokenKind::coloncolon, _contentIndex, 2);
						_advance(2);
						break;
					}

					if (_isBound(1) && _get(1) == ' ') {
						_append(TokenKind::colon, _contentIndex, 2);
						_advance(2);
						break;
					}
					
					size_t openingContextIndex = _contentIndex;
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
					_append(TokenKind::text, openingContextIndex, _contentIndex - openingContextIndex, Range {
						_oldLine, _oldColumn, _line, _column - 1
					});
					break;
				}

				case '"':
					_consumeQuote(
						(_tokens.size() != 0 && _tokens.back().kind == TokenKind::squoteOpen), // ‘“ case
						TokenKind::quoteOpen, 
						TokenKind::quoteClose
					);
					break;
					

				case '\'':
					_consumeQuote(
						(_tokens.size() != 0 && _tokens.back().kind == TokenKind::quoteOpen), // “‘ case
						TokenKind::squoteOpen, 
						TokenKind::squoteClose
					);
					break;

				case '\\':
					if (_isBound(1)) {
						_advance(1);
						_append(TokenKind::text, _contentIndex, 1);
						_advance(1);
						break;
					}
					_consumeText();
					break;

				case '\n': {
					size_t count = 0;

					while (_isBound(0) && _get(0) == '\n') {
						count += 1;
						_advance(1);
						_advanceLine(1);
					}

					Token token;
					token.range.startLine = _oldLine;
					token.range.startColumn = _oldColumn;
					token.range.endLine = _oldLine;
					token.range.endColumn = _oldColumn;

					if (count == 1) {
						token.kind = TokenKind::newline;
						_tokens.push_back(static_cast<Token&&>(token));
						_saveRangeStart();
						_consumeIndent();
						return;
					}

					token.kind = TokenKind::newlinePlus;
					_tokens.push_back(static_cast<Token&&>(token));
					_saveRangeStart();
					_consumeIndent();
					return;
				}

				case '@': {
					_advance(1);
					size_t oldContentIndex = _contentIndex;

					while (_isBound(0) && (
						(_get(0) >= 'a' && _get(0) <= 'z') ||
						(_get(0) >= 'A' && _get(0) <= 'Z') ||
						(_get(0) >= '0' && _get(0) <= '9') ||
						(_get(0) == '_')
					)) {
						_advance(1);
					}

					_append(TokenKind::accountTag, oldContentIndex, _contentIndex - oldContentIndex, Range { _oldLine, _oldColumn, _line, _column - 1 });
					break;
				}

				case '#': {
					_advance(1);
					size_t oldContentIndex = _contentIndex;

					while (_isBound(0) && (
						(_get(0) >= 'a' && _get(0) <= 'z') ||
						(_get(0) >= 'A' && _get(0) <= 'Z') ||
						(_get(0) >= '0' && _get(0) <= '9') ||
						(_get(0) == '_')
					)) {
						_advance(1);
					}

					_append(TokenKind::hashTag, oldContentIndex, _contentIndex - oldContentIndex, Range { _oldLine, _oldColumn, _line, _column - 1 });
					break;
				}

				default: 
					_consumeText(); 
					break;
			}
		}
	}

private:
	bool _isBound(size_t offset) const {
		return _contentIndex + offset < _content.size();
	}

	void _advance(size_t offset) {
		_contentIndex += offset;
		_column += offset;
	}

	void _advanceLine(size_t offset) {
		_line += offset;
		_column = 0;
	}

	void _saveRangeStart() {
		_oldLine = _line;
		_oldColumn = _column;
	}

	char _get(size_t offset) const {
		return _content[_contentIndex + offset];
	}

	void _append(TokenKind kind, size_t index = 0, size_t size = 0) {
		Token token;
		token.range.startLine = _oldLine;
		token.range.startColumn = _oldColumn;
		token.range.endLine = _line;
		token.range.endColumn = _column + (size == 0 ? 0 : size - 1);
		token.kind = kind;
		token.content = _content.substr(index, size);
		_tokens.push_back(static_cast<Token&&>(token));
	}

	void _append(TokenKind kind, size_t index, size_t size, Range range) {
		Token token;
		token.range = range;
		token.kind = kind;
		token.content = _content.substr(index, size);
		_tokens.push_back(static_cast<Token&&>(token));
	}

	void _consumeIndent() {
		size_t beginIndex = _contentIndex;
		size_t indentLevel = 0;
		while (_isBound(0) && _get(0) == '\t') {
			indentLevel += 1;
			_advance(1);
		}

		if (_indentLevel == indentLevel) {
			return _consumeBlockquote();
		}

		if (_indentLevel < indentLevel) {
			while (_indentLevel < indentLevel) {
				Token token;
				token.range.startLine = _oldLine;
				token.range.startColumn = _oldColumn;
				token.range.endLine = _oldLine;
				token.range.endColumn = _oldColumn;
				token.kind = TokenKind::indentOpen;
				_tokens.push_back(static_cast<Token&&>(token));
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
		size_t quoteLevel = 0;
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
				Token token;
				token.range.startLine = _oldLine;
				token.range.startColumn = _oldColumn;
				token.range.endLine = _oldLine;
				token.range.endColumn = _oldColumn;
				token.kind = TokenKind::blockquoteOpen;
				_tokens.push_back(static_cast<Token&&>(token));
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
			_content[_contentIndex - 1] == ' ' || 
            _content[_contentIndex - 1] == '\t' || 
            _content[_contentIndex - 1] == '\n' || 
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
		size_t beginIndex = _contentIndex + 1;
		
		if (_isBound(2) && _get(1) == '~' && _get(2) == ' ') {
			_advance(3);
			size_t keyIndex = _contentIndex;

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
				size_t valueIndex = _contentIndex;

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
			_advance(1);
		}

		Token token;
		token.range.startLine = _oldLine;
		token.range.startColumn = _oldColumn;
		token.range.endLine = _line;
		token.range.endColumn = _column - 1;
		token.kind = TokenKind::comment;
		token.content = _content.substr(beginIndex, _contentIndex - beginIndex);
		_tokens.push_back(static_cast<Token&&>(token));

		if (_isBound(0) && _get(0) == '\n') {
			_advance(1);
			_advanceLine(1);
		}
	}

	void _consumeText() {
		bool previousAlphanumeric = false;
		size_t beginIndex = _contentIndex;

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
				case '=':
				case '~':
				case '<':
				case '|':
				case '[':
				case ':':
				case '-':
				case '"':
				case '\'':
				case '\\':
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
					previousAlphanumeric = false;
					if (_isBound(1) && _get(1) == '[') {
						goto end;
					}

					_advance(1);
					break;

				case '^':
					previousAlphanumeric = false;
					if (_isBound(1) && (_get(1) == '[' || _get(1) == '(')) {
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

		Token token;
		token.range.startLine = _oldLine;
		token.range.startColumn = _oldColumn;
		token.range.endLine = _line;
		token.range.endColumn = _column - 1;
		token.kind = TokenKind::text;
		token.content = _content.substr(beginIndex, _contentIndex - beginIndex);
		_tokens.push_back(static_cast<Token&&>(token));

		return;
	}

	void _consumeIndex() {
		size_t beginIndex = _contentIndex;

		while (_isBound(0) && _get(0) >= '0' && _get(0) <= '9') {
			_advance(1);
		}

		if (_isBound(1) && _get(0) == '.' && _get(1) == ' ') {
			_append(TokenKind::index, beginIndex, _contentIndex - beginIndex + 1);
			_advance(2);
			return;
		}

		_contentIndex = beginIndex;
		_consumeText();
	}

	void _consumePipe() {
		while (_isBound(0)) {
			_saveRangeStart();

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
					_parseInline();
					break;
			}
		}
	}

	void _consumeLabel() {
		_append(TokenKind::parenOpen, _contentIndex, 1);
		_advance(1);
		_saveRangeStart();

		size_t oldContextIndex = _contentIndex;

		while (_isBound(0) && _get(0) != ')') {
			_advance(1);
		}

		_append(TokenKind::raw, oldContextIndex, _contentIndex - oldContextIndex, Range{
			_oldLine, _oldColumn, _line, _column - 1
		});
		_saveRangeStart();

		if (_isBound(0) && _get(0) == ')') {
			_append(TokenKind::parenClose, _contentIndex, 1);
			_advance(1);
		}
	}

	void _consumeLink() {
		_append(TokenKind::squareOpen, _contentIndex, 1);
		_advance(1);
		_saveRangeStart();

		size_t oldContextIndex = _contentIndex;

		while (_isBound(0) && _get(0) != ']') {
			_advance(1);
		}

		_append(TokenKind::raw, oldContextIndex, _contentIndex - oldContextIndex, Range{
			_oldLine, _oldColumn, _line, _column - 1
		});
		_saveRangeStart();

		if (_isBound(0) && _get(0) == ']') {
			_append(TokenKind::squareClose, _contentIndex, 1);
			_advance(1);
		}

		if (_isBound(1) && _get(0) == ':' && _get(1) == '\n') {
			_append(TokenKind::colon, _contentIndex, 1);
			_advance(1);
			_saveRangeStart();
		} else {
			if (_isBound(0) && _get(0) == '(') {
				_saveRangeStart();
				_consumeLabel();
			}
		}
	}

	void _consumeCode() {
		_append(TokenKind::backtick, _contentIndex, 1);
		_advance(1);
		_saveRangeStart();

		size_t oldContextIndex = _contentIndex;

		while (_isBound(0) && _get(0) != '`') {
			_advance(1);
		}

		Token token;
		token.range.startLine = _oldLine;
		token.range.startColumn = _oldColumn;
		token.range.endLine = _line;
		token.range.endColumn = _column - 1;
		token.kind = TokenKind::raw;
		_tokens.push_back(static_cast<Token&&>(token));

		_saveRangeStart();

		if (_isBound(0) && _get(0) == '`') {
			_append(TokenKind::backtick, _contentIndex, 1);
			_advance(1);
			_saveRangeStart();
		}
	}

	void _consumeCodeBlockContent(size_t dashCount) {
		size_t oldContextIndex = _contentIndex;
		size_t oldIndentLevel = _indentLevel;

		while (_isBound(0)) {
			if (_isBound(0) && _get(0) == '\n') {
				size_t oldColumn = _column;
				_advance(1);
				_advanceLine(1);
				size_t indentLevel = 0;
				while (_isBound(0) && _get(0) == '\t') {
					_advance(1);
					indentLevel += 1;
				}

				if (_get(0) == '-' && indentLevel == oldIndentLevel) {
					size_t i = 0;
					while (_isBound(0) && i < dashCount && _get(i) == '-') { i += 1; }
					if (i == dashCount && (!_isBound(dashCount) || (_isBound(dashCount) && _get(dashCount) == '\n'))) {
						size_t size = _contentIndex - oldContextIndex - oldIndentLevel;
						if (size > 0) {
							size -= 1;

							if (size > 0) {
								size -= 1;
							}
						}

						Token token;
						token.range.startLine = _oldLine;
						token.range.startColumn = _oldColumn;
						token.range.endLine = _line - 1;
						token.range.endColumn = oldColumn;
						token.kind = TokenKind::raw;
						token.content = _content.substr(oldContextIndex + 1, size);
						_tokens.push_back(static_cast<Token&&>(token));
						_saveRangeStart();

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
		size_t oldContentIndex = _contentIndex;

		while (_isBound(0) && _get(0) == '-') {
			_advance(1);
		}

		size_t dashCount = _contentIndex - oldContentIndex;

		if (_isBound(0) && _get(0) == '\n') {
			Token token;
			token.range.startLine = _oldLine;
			token.range.startColumn = _oldColumn;
			token.range.endLine = _line;
			token.range.endColumn = _column - 1;
			token.kind = TokenKind::fenceOpen;
			token.content = _content.substr(oldContentIndex, _contentIndex - oldContentIndex);
			_tokens.push_back(static_cast<Token&&>(token));
			_saveRangeStart();

			return _consumeCodeBlockContent(dashCount);
		}

		// capture label
		if (_isBound(1) && _get(0) == ' ' && _get(1) != '\n') {
			size_t oldColumn = _column;
			size_t youngContextIndex = _contentIndex;
			_advance(1);
			size_t middleAgedContentIndex = _contentIndex;

			while (_isBound(0) && _get(0) != '\n') {
				_advance(1);
			}

			if (_isBound(0) && _get(0) == '\n') {
				Token token;
				token.range.startLine = _oldLine;
				token.range.startColumn = _oldColumn;
				token.range.endLine = _line;
				token.range.endColumn = oldColumn - 1;
				token.kind = TokenKind::fenceOpen;
				token.content = _content.substr(oldContentIndex, youngContextIndex - oldContentIndex);
				_tokens.push_back(static_cast<Token&&>(token));
				_saveRangeStart();

				Token token2;
				token2.range.startLine = _oldLine;
				token2.range.startColumn = oldColumn + 1;
				token2.range.endLine = _line;
				token2.range.endColumn = _column - 1;
				token2.kind = TokenKind::text;
				token2.content = _content.substr(middleAgedContentIndex, _contentIndex - middleAgedContentIndex);
				_tokens.push_back(static_cast<Token&&>(token2));
				_saveRangeStart();

				return _consumeCodeBlockContent(dashCount);
			}
		}

		_contentIndex = oldContentIndex;
		_parseInline();
	}

private:
	std::string_view _content;

	size_t _contentIndex;

	size_t _line;

	size_t _column;

	size_t _oldLine;

	size_t _oldColumn;

	std::vector<Token> _tokens;

	size_t _indentLevel;

	size_t _quoteLevel;
};

}
