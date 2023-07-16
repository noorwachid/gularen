#include "Gularen/Token.h"
#include <Gularen/Lexer.h>
#include <iostream>
#include <fstream>

namespace Gularen {
	void Lexer::set(const std::string& content) {
		this->content = content;
	}

	void Lexer::tokenize() {
		if (content.empty()) return;

		this->tokens.clear();
		this->index = 0;
		this->position.line = 0;
		this->position.column = 0;

		tokenizeBlock();

		while (check(0)) {
			tokenizeInline();
		}

		if (!tokens.empty() && (tokens.back().type == TokenType::newline || tokens.back().type == TokenType::newlinePlus)) {
			tokens.back().type = TokenType::eof;
			tokens.back().value = "\\0";
			tokens.back().end = tokens.back().begin;
		} else {
			add(TokenType::eof, "\\0");
		}
	}

	const Tokens& Lexer::get() const {
		return tokens;
	}

	bool Lexer::check(size_t offset) {
		return index + offset < content.size();
	}

	bool Lexer::is(size_t offset, char c) {
		return content[index + offset] == c;
	}

	bool Lexer::isSymbol(size_t offset) {
		char c = content[index + offset];
		return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-';
	}

	char Lexer::get(size_t offset) {
		return content[index + offset];
	}

	void Lexer::advance(size_t offset) {
		index += 1 + offset;
		position.column += 1 + offset;
	}

	void Lexer::retreat(size_t offset) {
		index -= offset;
		position.column -= offset;
	}

	size_t Lexer::count(char c) {
		size_t count = 0;
		while (check(0) && is(0, c)) {
			++count;
			advance(0);
		}
		return count;
	}

	void Lexer::add(TokenType type, const std::string& value, Position begin, Position end) {
		Token token;
		token.type = type;
		token.value = value;
		token.begin = begin;
		token.end = end;
		tokens.push_back(token);
	}

	void Lexer::add(TokenType type, const std::string& value, Position begin) {
		Token token;
		token.type = type;
		token.value = value;
		token.begin = begin;
		token.end = position;
		--token.end.column;
		tokens.push_back(token);
	}

	void Lexer::add(TokenType type, const std::string& value) {
		Token token;
		token.type = type;
		token.value = value;
		token.begin = position;
		token.end = position;
		tokens.push_back(token);
	}

	void Lexer::addText(const std::string value) {
		if (!tokens.empty() && tokens.back().type == TokenType::text) {
			tokens.back().value += value;
			tokens.back().end.column += value.size();
		} else {
			if (!tokens.empty()) {
				Position beginPosition = position;
				beginPosition.column -= value.size() - 1;
				Token token;
				token.type = TokenType::text;
				token.value = value;
				token.begin = beginPosition;
				token.end = position;
				tokens.push_back(token);
			} else {
				Position beginPosition = position;
				beginPosition.column -= beginPosition.column > value.size() ? value.size() : 0;
				Token token;
				token.type = TokenType::text;
				token.value = value;
				token.begin = beginPosition;
				token.end = position;
				token.end.column -= 1;
				tokens.push_back(token);
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
				Position beginPosition = position;
				size_t counter = count('\n');

				if (counter == 1) {
					add(TokenType::newline, "\\n", beginPosition);
					position.line += counter;
					position.column = 0;
				} else {
					position.line += counter - 1;
					position.column = 0;
					Token token;
					token.type = TokenType::newlinePlus;
					token.value = "\\n";
					token.begin = beginPosition;
					token.end = position;
					tokens.push_back(token);
					position.line += 1;
				}

				tokenizeBlock();
				break;
			}

			case '\\':
				advance(0);
				if (check(0)) {
					addText(content.substr(index));
					advance(0);
				}
				break;

			case '~': {
				Position beginPosition = position;
				size_t commentIndex = index;
				size_t commentSize = 1;
				advance(0);
				while (check(0) && !is(0, '\n')) {
					++commentSize;
					advance(0);
				}
				add(TokenType::comment, content.substr(commentIndex, commentSize), beginPosition);
				break;
			}
			
			case '\'':
				tokenizeQuoMark(tokens.empty() || tokens.back().type == TokenType::ldQuo, TokenType::lsQuo, "‘", TokenType::rsQuo, "’");
				advance(0);
				break;

			case '"':
				tokenizeQuoMark(tokens.empty() || tokens.back().type == TokenType::lsQuo, TokenType::ldQuo, "“", TokenType::rdQuo, "”");
				advance(0);
				break;

			case '-': {
				if (check(2) && is(1, '-') && is(2, '-')) {
					add(TokenType::emDash, "–", Position(position.line, position.column), Position(position.line, position.column + 2));
					advance(2);
					break;
				}
				if (check(1) && is(1, '-')) {
					add(TokenType::enDash, "–", Position(position.line, position.column), Position(position.line, position.column + 1));
					advance(1);
					break;
				}
				add(TokenType::hyphen, "-");
				advance(0);
				break;
			}

			case ':': {
				Position emojiMarkerPosition = position;
				advance(0);

				if (check(0)) {
					size_t emojiIndex = index;
					size_t emojiSize = 0;

					while (check(0) && (get(0) >= 'a' && get(0) <= 'z' || is(0, '-'))) {
						if (is(0, ':')) {
							break;
						}

						++emojiSize;
						advance(0);
					}

					if (is(0, ':') && emojiSize > 0) {
						add(TokenType::emojiMarker, ":", emojiMarkerPosition, emojiMarkerPosition);
						add(TokenType::emojiCode, content.substr(emojiIndex, emojiSize), Position(emojiMarkerPosition.line, emojiMarkerPosition.column + 1));
						add(TokenType::emojiMarker, ":");
						advance(0);
						break;
					}

					addText(":" + content.substr(emojiIndex, emojiSize));
					break;
				}

				addText(":");
				break;
			}

			case '{': {
				Position beginPosition = position;

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

					add(TokenType::curlyOpen, "{", beginPosition, beginPosition);
					++beginPosition.column;
					add(TokenType::codeSource, source, beginPosition, Position(beginPosition.line, beginPosition.column + source.size() - 1));
					beginPosition.column += source.size();
					add(TokenType::curlyClose, "}", beginPosition, beginPosition);

					if (is(0, '(')) {
						advance(0);
						std::string lang;

						if (check(0) && isSymbol(0)) {
							while (check(0) && isSymbol(0)) {
								lang += get(0);
								advance(0);
							}

							if (is(0, ')')) {
								++beginPosition.column;
								add(TokenType::parenOpen, "(", beginPosition, beginPosition);
								++beginPosition.column;
								add(TokenType::resourceLabel, lang, beginPosition, Position(beginPosition.line, beginPosition.column + lang.size() - 1));
								beginPosition.column += lang.size();
								add(TokenType::parenClose, ")", beginPosition, beginPosition);
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
				Position beginPosition = position;

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
						add(TokenType::squareOpen, "[", beginPosition, beginPosition);
						++beginPosition.column;
						add(TokenType::resource, resource.substr(0, idMarkerIndex), beginPosition, Position(beginPosition.line, beginPosition.column + idMarkerIndex - 1));
						beginPosition.column += idMarkerIndex;
						add(TokenType::resourceIDMarker, ">", beginPosition, beginPosition);
						++beginPosition.column;
						add(TokenType::resourceID, resource.substr(idMarkerIndex + 1), beginPosition, Position(beginPosition.line, beginPosition.column + resource.size() - idMarkerIndex - 2));
						beginPosition.column += resource.size() - idMarkerIndex - 1;
						add(TokenType::squareClose, "]", beginPosition, beginPosition);
					} else {
						add(TokenType::squareOpen, "[", beginPosition, beginPosition);
						++beginPosition.column;
						add(TokenType::resource, resource, beginPosition, Position(beginPosition.line, beginPosition.column + resource.size() - 1));
						beginPosition.column += resource.size();
						add(TokenType::squareClose, "]", beginPosition, beginPosition);
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

							++beginPosition.column;
							add(TokenType::parenOpen, "(", beginPosition, beginPosition);
							++beginPosition.column;
							add(TokenType::resourceLabel, label, beginPosition, Position(beginPosition.line, beginPosition.column + label.size() - 1));
							beginPosition.column += label.size();
							add(TokenType::parenClose, ")", beginPosition, beginPosition);
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
					add(TokenType::presentMarker, "!");
					advance(0);
					// see inline [
					break;
				}
				advance(0);
				addText("!");
				break;

			case '?':
				if (check(1) && is(1, '[')) {
					add(TokenType::includeMarker, "?");
					advance(0);
					// see inline [
					break;
				}
				advance(0);
				addText("!");
				break;

			case '^':
			case '=': {
				Position beginPosition = position;
				std::string original(1, get(0));
				if (check(2) && is(1, '[') && isSymbol(2)) {
					advance(1);
					size_t idIndex = index;
					size_t idSize = 0;
					while (check(0) && isSymbol(0)) {
						++idSize;
						advance(0);
					}
					if (idSize > 0 && check(0) && is(0, ']')) {
						if (original[0] == '^') {
							add(TokenType::jumpMarker, original, beginPosition, beginPosition);
							++beginPosition.column;
							add(TokenType::squareOpen, "[", beginPosition, beginPosition);
							++beginPosition.column;
							add(TokenType::jumpID, content.substr(idIndex, idSize), beginPosition, Position(beginPosition.line, beginPosition.column + idSize - 1));
							beginPosition.column += idSize;
							++beginPosition.column;
							add(TokenType::squareClose, "]");
							advance(0);
							break;
						}
						if (check(1) && is(1, ' ')) {
							add(TokenType::describeMarker, original, beginPosition, beginPosition);
							++beginPosition.column;
							add(TokenType::squareOpen, "[", beginPosition, beginPosition);
							++beginPosition.column;
							add(TokenType::jumpID, content.substr(idIndex, idSize), beginPosition, Position(beginPosition.line, beginPosition.column + idSize - 1));
							beginPosition.column += idSize;
							++beginPosition.column;
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
				if (!tokens.empty() && tokens.back().type == TokenType::text) {
					size_t blankCount = 0;
					for (size_t i = tokens.back().value.size(); i >= 0 && tokens.back().value[i - 1] == ' '; --i) {
						++blankCount;
					}
					tokens.back().value = tokens.back().value.substr(0, tokens.back().value.size() - blankCount);
				}
				add(TokenType::pipe, "|");
				advance(0);
				tokenizeSpace();
				break;

				
			case '<':
				if (check(1) && is(1, '<')) {
					add(TokenType::break_, "<<", position, Position(position.line, position.column + 1));
					advance(1);
					break;
				}
				add(TokenType::break_, "<");
				advance(0);
				break;

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
				Position beginPosition = position;

				if (check(1) && is(1, ' ')) {
					advance(1);
					add(TokenType::headingIDOper, ">", beginPosition, beginPosition);

					size_t idIndex = index;
					size_t idSize = 0;

					while (check(0) && !is(0, '\n') && isSymbol(0)) {
						++idSize;
						advance(0);
					}
					
					if (idSize > 0) {
						add(TokenType::headingID, content.substr(idIndex, idSize));
					}
					break;
				}

				addText(">");
				advance(0);
				break;
			}

			default:
				addText(content.substr(index, 1));
				advance(0);
				break;
		}
	}

	void Lexer::tokenizePrefix() {
		Position beginPosition = position;

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

			if (
				(is(0, '/') && is(1, '\0')) ||
				(is(0, '/') && is(1, '\n')) ||
				(is(0, '/') && is(1, '\t'))
			) {
				currentPrefix += TokenType::bqIncr;
				advance(0);
				continue;
			}

			break;
		}

		size_t lowerBound = std::min(prefix.size(), currentPrefix.size());

		for (size_t i = 0; i < lowerBound; ++i) {
			if (currentPrefix[i] != prefix[i]) {
				lowerBound = i;
				break;
			}
		}

		if (prefix.size() > lowerBound) {
			while (prefix.size() > lowerBound) {
				add(
					prefix.back() == TokenType::indentIncr ? TokenType::indentDecr : TokenType::bqDecr, 
					prefix.back() == TokenType::indentIncr ? "I-" : "B-", 
					beginPosition, 
					beginPosition
				);
				prefix.pop_back();
			}
		}

		if (currentPrefix.size() > lowerBound) {
			while (lowerBound < currentPrefix.size()) {
				add(
					currentPrefix[lowerBound],
					currentPrefix[lowerBound] == TokenType::indentIncr ? "I+" : "B+", 
					beginPosition, 
					beginPosition
				);
				++lowerBound;
			}
		}

		prefix = currentPrefix;
		indent = currentIndent;

		/* size_t currentIndent = is(0, '\t') ? count('\t') : 0; */

		/* if (currentIndent > indent) { */
		/* 	while (currentIndent > indent) { */
		/* 		beginPosition.column = indent; */
		/* 		add(TokenType::indentIncr, "I+", beginPosition, beginPosition); */
		/* 		++indent; */
		/* 	} */
		/* } */

		/* if (currentIndent < indent) { */
		/* 	while (currentIndent < indent) { */
		/* 		--indent; */
		/* 		// indentDecr has no symbol */
		/* 		add(TokenType::indentDecr, "I-", tokens.back().begin, tokens.back().begin); */
		/* 	} */
		/* } */
	}

	void Lexer::tokenizeBlock() {
		tokenizePrefix();

		Position beginPosition = position;

		switch (get(0)) {
			case '>': {
				Position beginPosition = position;

				size_t counter = count('>');
				if (counter > 3) {
					addText(std::string(counter, '>'));
					break;
				}

				if (is(0, ' ')) {
					advance(0);
					switch (counter) {
						case 3:
							add(TokenType::chapterOper, ">>>", beginPosition, Position(beginPosition.line, beginPosition.column + 2));
							break;
						case 2:
							add(TokenType::sectionOper, ">>", beginPosition, Position(beginPosition.line, beginPosition.column + 1));
							break;
						case 1:
							add(TokenType::subsectionOper, ">", beginPosition, beginPosition);
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
				Position beginPosition = position;
				if (check(3) && is(2, ']') && is(3, ' ')) {
					if (is(1, ' ')) {
						add(TokenType::checkbox, "[ ]", beginPosition, Position(beginPosition.line, beginPosition.column + 2));
						advance(3);
						break;
					}
					if (is(1, 'v')) {
						add(TokenType::checkbox, "[v]", beginPosition, Position(beginPosition.line, beginPosition.column + 2));
						advance(2);
						break;
					}
					if (is(1, 'x')) {
						add(TokenType::checkbox, "[x]", beginPosition, Position(beginPosition.line, beginPosition.column + 2));
						advance(2);
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

			case '<':
				if (check(3) && is(2, '>') && is(3, ' ')) {
					Position beginPosition = position;
					// </> Note
					// <?> Hint
					// <!> Important
					// <^> Warning
					// <&> See also
					// <+> Tip
					if (is(1, '/')) {
						advance(2);
						tokenizeSpace();
						add(TokenType::admonNote, "</>", beginPosition);
						break;
					}
					if (is(1, '?')) {
						advance(2);
						tokenizeSpace();
						add(TokenType::admonHint, "<?>", beginPosition);
						break;
					}
					if (is(1, '!')) {
						advance(2);
						tokenizeSpace();
						add(TokenType::admonImportant, "<!>", beginPosition);
						break;
					}
					if (is(1, '^')) {
						advance(2);
						tokenizeSpace();
						add(TokenType::admonWarning, "<^>", beginPosition);
						break;
					}
					if (is(1, '&')) {
						advance(2);
						tokenizeSpace();
						add(TokenType::admonSeeAlso, "<&>", beginPosition);
						break;
					}
					if (is(1, '+')) {
						advance(2);
						tokenizeSpace();
						add(TokenType::admonTip, "<+>", beginPosition);
						break;
					}
				}
				// see inline <
				break;

			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				Position beginPosition = position;
				std::string number;
				while (check(0) && get(0) >= '0' && get(0) <= '9') {
					number += get(0);
					advance(0);
				}

				if (check(1) && is(0, '.') && is(1, ' ')) {
					advance(0);
					tokenizeSpace();
					add(TokenType::index, number + ".", beginPosition, Position(beginPosition.line, beginPosition.column + number.size()));
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
		Position beginPosition = position;

		size_t openingIndent = indent;
		size_t openingCounter = count('-');

		if (openingCounter == 1) {
			if (is(0, ' ')) {
				advance(0);
				add(TokenType::bullet, "-", beginPosition, beginPosition);
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
			size_t langIndex = index;
			size_t langSize = 0;

			Position beginLangPosition = position;

			while (check(0) && !is(0, '\n') && isSymbol(0)) {
				++langSize;
				advance(0);
			}
			
			if (langSize == 0 || !is(0, '\n')) {
				retreat(openingCounter + spaceCounter + langSize);
				return;
			}

			add(TokenType::codeMarker, std::string(openingCounter, '-'), beginPosition, Position(beginLangPosition.line, beginLangPosition.column - 1));
			add(TokenType::codeLang, content.substr(langIndex, langSize), beginLangPosition);
		} else {
			if (!is(0, '\n')) {
				retreat(openingCounter + spaceCounter);
				// see inline -
				return;
			}

			add(TokenType::codeMarker, std::string(openingCounter, '-'), beginPosition);
		}

		beginPosition = position;
		beginPosition.line += 1;
		beginPosition.column = indent;

		Position endPosition = beginPosition;

		std::string source;
		position.column = indent;

		while (check(0)) {
			if (is(0, '\n')) {
				endPosition = position;

				size_t newline = count('\n');
				size_t indent = count('\t');

				position.line += newline;
				position.column = indent;

				if (check(2) && is(0, '-') && is(1, '-') && is(2, '-')) {
					size_t closingCounter = count('-');
					if ((index >= content.size() || check(0) && is(0, '\n')) && closingCounter == openingCounter) {
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

		add(TokenType::codeSource, source.substr(trimBegin, trimSize), beginPosition, endPosition);
		add(TokenType::codeMarker, std::string(openingCounter, '-'), Position(position.line, indent));
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
			Position beginPosition = position;

			if (check(1) && is(0, ':') && is(1, '-')) {
				advance(0);
				size_t lineCounter = count('-');

				if (get(0) == ':') {
					advance(0);
					add(TokenType::pipeConnector, ":-:", beginPosition);
					continue;
				}

				add(TokenType::pipeConnector, ":--", beginPosition);
				continue;
			}

			if (is(0, '-')) {
				size_t lineCounter = count('-');

				if (get(0) == ':') {
					advance(0);
					add(TokenType::pipeConnector, "--:", beginPosition);
					continue;
				}

				add(TokenType::pipeConnector, ":--", beginPosition);
				continue;
			}

			if (is(0, '|')) {
				add(TokenType::pipe, "|");
				advance(0);
				continue;
			}

			addText(content.substr(index));
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
					addText(content.substr(index, 1));
					advance(0);
					break;

				default:
					return;
			}
		}
	}

	void Lexer::tokenizeQuoMark(bool should, TokenType leftType, const std::string& leftValue, TokenType rightType, const std::string& rightValue) {
		if (should || 
			index == 0 ||
			content[index - 1] == ' ' || 
            content[index - 1] == '\t' || 
            content[index - 1] == '\n' || 
            content[index - 1] == '\0')
            
        {
            add(leftType, leftValue);
            return;
        }

        add(rightType, rightValue);
	}
}
