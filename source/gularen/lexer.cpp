#include "gularen/lexer.h"
#include "gularen/token.h"
#include <fstream>
#include <iostream>

namespace Gularen {
	bool IsDigit(char letter) {
		return letter >= '0' && letter <= '9';
	}

	bool IsDate(std::string_view text) {
		if (text.size() == 10 && IsDigit(text[0]) && IsDigit(text[1]) && IsDigit(text[2]) && IsDigit(text[3]) &&
			text[4] == '-' && IsDigit(text[5]) && IsDigit(text[6]) && text[7] == '-' && IsDigit(text[8]) &&
			IsDigit(text[9])) {
			return true;
		}
		return false;
	}

	bool IsTime(std::string_view text) {
		if (text.size() == 5 && IsDigit(text[0]) && IsDigit(text[1]) && text[2] == ':' && IsDigit(text[3]) &&
			IsDigit(text[4])) {
			return true;
		}

		if (text.size() == 8 && IsDigit(text[0]) && IsDigit(text[1]) && text[2] == ':' && IsDigit(text[3]) &&
			IsDigit(text[4]) && text[5] == ':' && IsDigit(text[6]) && IsDigit(text[7])) {
			return true;
		}

		return false;
	}

	bool IsDateTime(std::string_view text) {
		if (text.size() == 16 && IsDate(text.substr(0, 10)) && IsTime(text.substr(11, 5))) {
			return true;
		}
		if (text.size() == 19 && IsDate(text.substr(0, 10)) && IsTime(text.substr(11, 8))) {
			return true;
		}
		return false;
	}

	void Lexer::set(const std::string& content) {
		this->_content = content;
	}

	void Lexer::tokenize() {
		if (_content.empty())
			return;

		_tokens.clear();
		_index = 0;
		_end.line = 1;
		_end.column = 1;
		_begin.line = 1;
		_begin.column = 1;

		tokenizeBlock();

		while (check(0)) {
			tokenizeInline();
		}

		if (!_tokens.empty() &&
			(_tokens.back().type == TokenType::newline || _tokens.back().type == TokenType::newlinePlus)) {
			_tokens.back().type = TokenType::end;
			_tokens.back().value = "\\0";
			_tokens.back().range.end = _tokens.back().range.begin;
		} else {
			add(TokenType::end, "\\0");
		}
	}

	const Tokens& Lexer::get() const {
		return _tokens;
	}

	bool Lexer::check(size_t offset) {
		return _index + offset < _content.size();
	}

	bool Lexer::is(size_t offset, char c) {
		return _content[_index + offset] == c;
	}

	bool Lexer::isSymbol(size_t offset) {
		char c = _content[_index + offset];
		return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-';
	}

	char Lexer::get(size_t offset) {
		return _content[_index + offset];
	}

	void Lexer::advance(size_t offset) {
		_index += 1 + offset;
		_end.column += 1 + offset;
	}

	void Lexer::retreat(size_t offset) {
		_index -= offset;
		_end.column -= offset;
	}

	size_t Lexer::count(char c) {
		size_t count = 0;
		while (check(0) && is(0, c)) {
			++count;
			advance(0);
		}
		return count;
	}

	void Lexer::add(TokenType type, const std::string& value) {
		if (!_tokens.empty() && _tokens.back().type == TokenType::text)
			_begin.column += 1;

		Token token;
		token.type = type;
		token.value = value;
		token.range.begin = _begin;
		token.range.end = _end;
		_tokens.push_back(token);
		_begin = _end;
		_begin.column += 1;
	}

	void Lexer::addText(const std::string value) {
		if (!_tokens.empty() && _tokens.back().type == TokenType::text) {
			_tokens.back().value += value;
			_tokens.back().range.end.column += value.size();
			_begin = _end;
		} else {
			if (!_tokens.empty()) {
				Token token;
				token.type = TokenType::text;
				token.value = value;
				token.range.begin = _begin;
				token.range.end = _end;
				_tokens.push_back(token);
				_begin = _end;
			} else {
				Token token;
				token.type = TokenType::text;
				token.value = value;
				token.range.begin = _begin;
				token.range.end = _end;
				_tokens.push_back(token);
				_begin = _end;
			}
		}
	}

	void Lexer::tokenizeInline() {
		switch (get(0)) {
			case ' ':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
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
				tokenizeText();
				break;

			case '\n': {
				size_t originalColumn = _end.column;
				size_t counter = count('\n');

				if (counter == 1) {
					Token token;
					token.type = TokenType::newline;
					token.value = "\\n";
					_end.column = originalColumn;
					token.range.begin = _end;
					token.range.end = _end;
					_end.line += 1;
					_end.column = 1;
					_begin = _end;
					_tokens.push_back(token);
				} else {
					_end.line += counter - 1;
					_end.column = 1;
					Token token;
					token.type = TokenType::newlinePlus;
					token.value = "\\n";
					token.range.begin = _begin;
					token.range.end = _end;
					_end.line += 1;
					_begin = _end;
					_tokens.push_back(token);
				}

				tokenizeBlock();
				break;
			}

			case '\\':
				advance(0);
				if (check(0)) {
					addText(_content.substr(_index, 1));
					advance(0);
				}
				break;

			case '~': {
				size_t commentIndex = _index;
				size_t commentSize = 1;
				advance(0);
				while (check(0) && !is(0, '\n')) {
					++commentSize;
					advance(0);
				}
				add(TokenType::comment, _content.substr(commentIndex, commentSize));
				break;
			}

			case '\'':
				tokenizeQuoMark(
					_tokens.empty() || _tokens.back().type == TokenType::quoteOpen, TokenType::singleQuoteOpen, "‘", TokenType::singleQuoteClose,
					"’"
				);
				advance(0);
				break;

			case '"':
				tokenizeQuoMark(
					_tokens.empty() || _tokens.back().type == TokenType::singleQuoteOpen, TokenType::quoteOpen, "“", TokenType::quoteClose,
					"”"
				);
				advance(0);
				break;

			case '-': {
				if (check(2) && is(1, '-') && is(2, '-')) {
					add(TokenType::emDash, "–");
					advance(2);
					break;
				}
				if (check(1) && is(1, '-')) {
					add(TokenType::enDash, "–");
					advance(1);
					break;
				}
				add(TokenType::hyphen, "-");
				advance(0);
				break;
			}

			case ':': {
				advance(0);

				if (check(0)) {
					size_t emojiIndex = _index;
					size_t emojiSize = 0;

					while (check(0) && (get(0) >= 'a' && get(0) <= 'z' || is(0, '-'))) {
						if (is(0, ':')) {
							break;
						}

						++emojiSize;
						advance(0);
					}

					if (is(0, ':') && emojiSize > 0) {
						add(TokenType::emojiMark, ":");
						add(TokenType::emojiCode, _content.substr(emojiIndex, emojiSize));
						add(TokenType::emojiMark, ":");
						advance(0);
						break;
					}

					addText(":" + _content.substr(emojiIndex, emojiSize));
					break;
				}

				addText(":");
				break;
			}

			case '{': {
				advance(0);

				if (check(0)) {
					size_t depth = 0;
					std::string source;

					while (check(0)) {
						if (is(0, '}') && depth == 0) {
							advance(0);
							break;
						}

						if (is(0, '{')) {
							++depth;
						}

						if (is(0, '\\')) {
							advance(0);
						}

						source += get(0);
						advance(0);
					}

					add(TokenType::curlyOpen, "{");
					add(TokenType::codeSource, source);
					add(TokenType::curlyClose, "}");

					if (is(0, '(')) {
						advance(0);
						std::string lang;

						if (check(0) && isSymbol(0)) {
							while (check(0) && isSymbol(0)) {
								lang += get(0);
								advance(0);
							}

							if (is(0, ')')) {
								add(TokenType::parenOpen, "(");
								add(TokenType::resourceLabel, lang);
								add(TokenType::parenClose, ")");
								advance(0);
								break;
							}

							addText("(" + lang);
							break;
						} else {
							addText("(");
						}
					}
					break;
				}

				addText("{");
			}

			case '[': {
				advance(0);

				if (check(0)) {
					bool idMarkerExists = false;
					size_t idMarkerIndex = 0;
					size_t depth = 0;
					std::string resource;

					while (check(0)) {
						if (is(0, ']') && depth == 0) {
							advance(0);
							break;
						}

						if (is(0, '[')) {
							++depth;
						}

						if (is(0, '>')) {
							idMarkerExists = true;
							idMarkerIndex = resource.size();
						}

						if (is(0, '\\')) {
							advance(0);
						}

						resource += get(0);
						advance(0);
					}

					if (idMarkerExists) {
						add(TokenType::squareOpen, "[");
						add(TokenType::resource, resource.substr(0, idMarkerIndex));
						add(TokenType::resourceIDMark, ">");
						add(TokenType::resourceID, resource.substr(idMarkerIndex + 1));
						add(TokenType::squareClose, "]");
					} else {
						add(TokenType::squareOpen, "[");
						add(TokenType::resource, resource);
						add(TokenType::squareClose, "]");
					}

					if (is(0, '(')) {
						advance(0);

						if (check(0)) {
							size_t depth = 0;
							std::string label;

							while (check(0)) {
								if (is(0, ')') && depth == 0) {
									advance(0);
									break;
								}

								if (is(0, '(')) {
									++depth;
								}

								if (is(0, '\\')) {
									advance(0);
								}

								label += get(0);
								advance(0);
							}

							add(TokenType::parenOpen, "(");
							add(TokenType::resourceLabel, label);
							add(TokenType::parenClose, ")");
							break;
						} else {
							addText("(");
						}
					}
					break;
				}

				addText("[");
				break;
			}

			case '!':
				if (check(1) && is(1, '[')) {
					add(TokenType::presentMark, "!");
					advance(0);
					// see inline [
					break;
				}
				advance(0);
				addText("!");
				break;

			case '?':
				if (check(1) && is(1, '[')) {
					add(TokenType::includeMark, "?");
					advance(0);
					// see inline [
					break;
				}
				advance(0);
				addText("?");
				break;

			case '^':
			case '=': {
				std::string original(1, get(0));
				if (check(2) && is(1, '[') && isSymbol(2)) {
					advance(1);
					size_t idIndex = _index;
					size_t idSize = 0;
					while (check(0) && isSymbol(0)) {
						++idSize;
						advance(0);
					}
					if (idSize > 0 && check(0) && is(0, ']')) {
						if (original[0] == '^') {
							add(TokenType::jumpMark, original);
							add(TokenType::squareOpen, "[");
							add(TokenType::jumpID, _content.substr(idIndex, idSize));
							add(TokenType::squareClose, "]");
							advance(0);
							break;
						}
						if (check(1) && is(1, ' ')) {
							add(TokenType::describeMark, original);
							add(TokenType::squareOpen, "[");
							add(TokenType::jumpID, _content.substr(idIndex, idSize));
							add(TokenType::squareClose, "]");
							advance(1);
							break;
						}
					}
					addText(original);
					retreat(idSize + 1);
					break;
				}
				advance(0);
				addText(original);
				break;
			}

			case '|':
				// trim right
				if (!_tokens.empty() && _tokens.back().type == TokenType::text) {
					size_t blankCount = 0;
					for (size_t i = _tokens.back().value.size(); i >= 0 && _tokens.back().value[i - 1] == ' '; --i) {
						++blankCount;
					}
					_tokens.back().value = _tokens.back().value.substr(0, _tokens.back().value.size() - blankCount);
				}
				add(TokenType::pipe, "|");
				advance(0);
				tokenizeSpace();
				break;

			case '<': {
				if (check(2) && is(1, '<') && is(2, '<')) {
					add(TokenType::break_, "<<<");
					advance(2);
					break;
				}
				if (check(1) && is(1, '<')) {
					add(TokenType::break_, "<<");
					advance(1);
					break;
				}
				advance(0);
				size_t begin = _index;
				size_t size = 0;
				while (check(0) && !is(0, '>')) {
					++size;
					advance(0);
				}
				advance(0);
				std::string_view text = std::string_view(_content.data() + begin, size);

				if (IsDate(text)) {
					add(TokenType::date, _content.substr(begin, size));
					break;
				}
				if (IsTime(text)) {
					add(TokenType::time, _content.substr(begin, size));
					break;
				}
				if (IsDateTime(text)) {
					add(TokenType::dateTime, _content.substr(begin, size));
					break;
				}

				addText("<");
				addText(_content.substr(begin, size));
				addText(">");
				break;
			}

			case '*':
				if (check(2) && is(1, '*') && is(2, '*')) {
					add(TokenType::dinkus, "***");
					advance(2);
					break;
				}
				add(TokenType::fsBold, "*");
				advance(0);
				break;

			case '_':
				add(TokenType::fsItalic, "_");
				advance(0);
				break;

			case '`':
				add(TokenType::fsMonospace, "`");
				advance(0);
				break;

			case '>': {
				if (check(1) && is(1, ' ')) {
					add(TokenType::headingIDMark, ">");
					advance(1);

					size_t idIndex = _index;
					size_t idSize = 0;

					while (check(0) && !is(0, '\n') && isSymbol(0)) {
						++idSize;
						advance(0);
					}

					if (idSize > 0) {
						add(TokenType::headingID, _content.substr(idIndex, idSize));
					}
					break;
				}

				addText(">");
				advance(0);
				break;
			}

			default:
				addText(_content.substr(_index, 1));
				advance(0);
				break;
		}
	}

	void Lexer::tokenizePrefix() {
		std::basic_string<TokenType> currentPrefix;
		size_t currentIndent = 0;

		while (is(0, '\t') || is(0, '/')) {
			if (is(0, '\t')) {
				currentPrefix += TokenType::indentIncr;
				++currentIndent;
				advance(0);
				continue;
			}

			if (check(1) && is(0, '/') && is(1, ' ')) {
				currentPrefix += TokenType::bqIncr;
				advance(1);
				continue;
			}

			if ((is(0, '/') && is(1, '\0')) || (is(0, '/') && is(1, '\n')) || (is(0, '/') && is(1, '\t'))) {
				currentPrefix += TokenType::bqIncr;
				advance(0);
				continue;
			}

			break;
		}

		size_t lowerBound = std::min(_prefix.size(), currentPrefix.size());

		for (size_t i = 0; i < lowerBound; ++i) {
			if (currentPrefix[i] != _prefix[i]) {
				lowerBound = i;
				break;
			}
		}

		if (_prefix.size() > lowerBound) {
			while (_prefix.size() > lowerBound) {
				add(_prefix.back() == TokenType::indentIncr ? TokenType::indentDecr : TokenType::bqDecr,
					_prefix.back() == TokenType::indentIncr ? "I-" : "B-");
				_prefix.pop_back();
			}
		}

		if (currentPrefix.size() > lowerBound) {
			while (lowerBound < currentPrefix.size()) {
				add(currentPrefix[lowerBound], currentPrefix[lowerBound] == TokenType::indentIncr ? "I+" : "B+");
				++lowerBound;
			}
		}

		_prefix = currentPrefix;
		_indent = currentIndent;
	}

	void Lexer::tokenizeBlock() {
		tokenizePrefix();

		switch (get(0)) {
			case '>': {
				size_t counter = count('>');
				if (counter > 3) {
					addText(std::string(counter, '>'));
					break;
				}

				if (is(0, ' ')) {
					advance(0);
					switch (counter) {
						case 3:
							add(TokenType::chapterMark, ">>>");
							break;
						case 2:
							add(TokenType::sectionMark, ">>");
							break;
						case 1:
							add(TokenType::subsectionMark, ">");
							break;
					}
					break;
				}
				addText(std::string(counter, '>'));
				break;
			}

			case '-': {
				tokenizeCode();
				break;
			}

			case '[': {
				if (check(3) && is(2, ']') && is(3, ' ')) {
					if (is(1, ' ')) {
						add(TokenType::checkbox, "[ ]");
						advance(2);
						tokenizeSpace();
						break;
					}
					if (is(1, 'v')) {
						add(TokenType::checkbox, "[v]");
						advance(2);
						tokenizeSpace();
						break;
					}
					if (is(1, 'x')) {
						add(TokenType::checkbox, "[x]");
						advance(2);
						tokenizeSpace();
						break;
					}
				}

				// see inline [
				break;
			}

			case '{': {
				// see inline [
				break;
			}

			case '|':
				tokenizeTable();
				break;

			case '<': {
				size_t previousIndex = _index;
				advance(0);
				size_t beginIndex = _index;
				size_t size = 0;
				while (check(0) && !is(0, '>')) {
					++size;
					advance(0);
				}
				advance(0);
				std::string inside = _content.substr(beginIndex, size);
				if (inside == "note") {
					add(TokenType::admonNote, "<note>");
					tokenizeSpace();
					break;
				} else if (inside == "hint") {
					add(TokenType::admonHint, "<hint>");
					tokenizeSpace();
					break;
				} else if (inside == "important") {
					add(TokenType::admonImportant, "<important>");
					tokenizeSpace();
					break;
				} else if (inside == "warning") {
					add(TokenType::admonWarning, "<warning>");
					tokenizeSpace();
					break;
				} else if (inside == "seealso") {
					add(TokenType::admonSeeAlso, "<seealso>");
					tokenizeSpace();
					break;
				} else if (inside == "tip") {
					add(TokenType::admonTip, "<tip>");
					tokenizeSpace();
					break;
				}

				_index = previousIndex;
				// see inline <
				break;
			}

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				std::string number;
				while (check(0) && get(0) >= '0' && get(0) <= '9') {
					number += get(0);
					advance(0);
				}

				if (check(1) && is(0, '.') && is(1, ' ')) {
					advance(0);
					tokenizeSpace();
					add(TokenType::index, number + ".");
					break;
				}

				addText(number);
				break;
			}

			default:
				break;
		}
	}

	void Lexer::tokenizeCode() {
		size_t openingIndent = _indent;
		size_t openingCounter = count('-');

		if (openingCounter == 1) {
			if (is(0, ' ')) {
				advance(0);
				add(TokenType::bullet, "-");
				return;
			}

			retreat(1);
			// see inline -
			return;
		}

		if (openingCounter == 2) {
			retreat(2);
			// see inline -
			return;
		}

		bool hasSpace = is(0, ' ');
		size_t spaceCounter = count(' ');

		if (hasSpace) {
			size_t langIndex = _index;
			size_t langSize = 0;

			while (check(0) && !is(0, '\n') && isSymbol(0)) {
				++langSize;
				advance(0);
			}

			if (langSize == 0 || !is(0, '\n')) {
				retreat(openingCounter + spaceCounter + langSize);
				return;
			}

			add(TokenType::codeMark, std::string(openingCounter, '-'));
			add(TokenType::codeLang, _content.substr(langIndex, langSize));
		} else {
			if (!is(0, '\n')) {
				retreat(openingCounter + spaceCounter);
				// see inline -
				return;
			}

			add(TokenType::codeMark, std::string(openingCounter, '-'));
		}

		std::string source;
		_end.column = _indent;

		while (check(0)) {
			if (is(0, '\n')) {
				size_t newline = count('\n');
				size_t indent = count('\t');

				_end.line += newline;
				_end.column = indent;

				if (check(2) && is(0, '-') && is(1, '-') && is(2, '-')) {
					size_t closingCounter = count('-');
					if ((_index >= _content.size() || check(0) && is(0, '\n')) && closingCounter == openingCounter) {
						break;
					}

					if (newline > 0) {
						source += std::string(newline, '\n');
					}

					if (indent > openingIndent) {
						source += std::string(indent - openingIndent, '\t');
					}

					source += std::string(closingCounter, '-');
					continue;
				} else {
					if (newline > 0) {
						source += std::string(newline, '\n');
					}

					if (indent > openingIndent) {
						source += std::string(indent - openingIndent, '\t');
					}
					continue;
				}
			}

			source += get(0);
			advance(0);
		}

		size_t trimBegin = 0;
		size_t trimEnd = 0;

		for (size_t i = 0; i < source.size() && source[i] == '\n'; ++i) {
			++trimBegin;
		}

		for (size_t i = source.size(); source.size() && i > 0 && source[i] == '\n'; --i) {
			++trimEnd;
		}

		size_t trimSize = trimEnd == source.size() ? source.size() : source.size() - trimBegin - trimEnd;

		add(TokenType::codeSource, source.substr(trimBegin, trimSize));
		add(TokenType::codeMark, std::string(openingCounter, '-'));
	}

	void Lexer::tokenizeSpace() {
		while (check(0) && is(0, ' ')) {
			advance(0);
		}

		return;
	}

	void Lexer::tokenizeTable() {
		add(TokenType::pipe, "|");
		advance(0);

		if (!(is(0, '-') || is(0, ':'))) {
			return;
		}

		while (check(0) && (is(0, '-') || is(0, ':') || is(0, '|')) && !is(0, '\n')) {
			if (check(1) && is(0, ':') && is(1, '-')) {
				advance(0);
				size_t lineCounter = count('-');

				if (get(0) == ':') {
					advance(0);
					add(TokenType::pipeConnector, ":-:");
					continue;
				}

				add(TokenType::pipeConnector, ":--");
				continue;
			}

			if (is(0, '-')) {
				size_t lineCounter = count('-');

				if (get(0) == ':') {
					advance(0);
					add(TokenType::pipeConnector, "--:");
					continue;
				}

				add(TokenType::pipeConnector, ":--");
				continue;
			}

			if (is(0, '|')) {
				add(TokenType::pipe, "|");
				advance(0);
				continue;
			}

			addText(_content.substr(_index));
			return;
		}
	}

	void Lexer::tokenizeText() {
		while (check(0)) {
			switch (get(0)) {
				case ' ':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				case 'g':
				case 'h':
				case 'i':
				case 'j':
				case 'k':
				case 'l':
				case 'm':
				case 'n':
				case 'o':
				case 'p':
				case 'q':
				case 'r':
				case 's':
				case 't':
				case 'u':
				case 'v':
				case 'w':
				case 'x':
				case 'y':
				case 'z':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'G':
				case 'H':
				case 'I':
				case 'J':
				case 'K':
				case 'L':
				case 'M':
				case 'N':
				case 'O':
				case 'P':
				case 'Q':
				case 'R':
				case 'S':
				case 'T':
				case 'U':
				case 'V':
				case 'W':
				case 'X':
				case 'Y':
				case 'Z':
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
					addText(_content.substr(_index, 1));
					advance(0);
					break;

				default:
					return;
			}
		}
	}

	void Lexer::tokenizeQuoMark(
		bool should, TokenType leftType, const std::string& leftValue, TokenType rightType,
		const std::string& rightValue
	) {
		if (should || _index == 0 || _content[_index - 1] == ' ' || _content[_index - 1] == '\t' ||
			_content[_index - 1] == '\n' || _content[_index - 1] == '\0')

		{
			add(leftType, leftValue);
			return;
		}

		add(rightType, rightValue);
	}
}
