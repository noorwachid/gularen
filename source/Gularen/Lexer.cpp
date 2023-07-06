#include <Gularen/Lexer.h>
#include <iostream>
#include <fstream>

namespace Gularen {
	void Lexer::set(const std::string& content) {
		this->content = content;
	}

	void Lexer::parse() {
		if (content.empty()) return;

		this->tokens.clear();
		this->index = 0;
		this->position.line = 0;
		this->position.column = 0;

		parseBlock();

		while (check(0)) {
			parseInline();
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
			Position beginPosition = position;
			beginPosition.column -= value.size() - 1;
			Token token;
			token.type = TokenType::text;
			token.value = value;
			token.begin = beginPosition;
			token.end = position;
			tokens.push_back(token);
		}
	}

	void Lexer::parseInline() {
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
				parseText();
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

				parseBlock();
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
				parseQuoMark(tokens.empty() || tokens.back().type == TokenType::ldQuo, TokenType::lsQuo, "‘", TokenType::rsQuo, "’");
				advance(0);
				break;

			case '"':
				parseQuoMark(tokens.empty() || tokens.back().type == TokenType::lsQuo, TokenType::ldQuo, "“", TokenType::rdQuo, "”");
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
				Position emojiDeliPosition = position;
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
						add(TokenType::emojiDeli, ":", emojiDeliPosition);
						add(TokenType::emoji, content.substr(emojiIndex, emojiSize));
						advance(0);
						add(TokenType::emojiDeli, ":");
						break;
					}

					addText(":" + content.substr(emojiIndex, emojiSize));
					break;
				}

				addText(":");
				break;
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
						add(TokenType::resource, resource.substr(0, idMarkerIndex));
						add(TokenType::resourceID, resource.substr(idMarkerIndex + 1));
					} else {
						add(TokenType::resource, resource);
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

							add(TokenType::resourceLabel, label);
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
					advance(0);
					add(TokenType::presentMarker, "!");
					// see inline [
					break;
				}
				advance(0);
				addText("!");
				break;

			case '?':
				if (check(1) && is(1, '[')) {
					advance(0);
					add(TokenType::includeMarker, "?");
					// see inline [
					break;
				}
				advance(0);
				addText("!");
				break;

			case '^':
			case '=': {
				std::string original(1, get(0));
				if (check(2) && is(1, '[') && isSymbol(2)) {
					advance(1);
					size_t idIndex = 0;
					size_t idSize = 0;
					while (check(0) && isSymbol(0)) {
						++idSize;
						advance(0);
					}
					if (idSize > 0 && check(0) && is(0, ']')) {
						add(original == "^" ? TokenType::jumpMarker : TokenType::describeMarker, original);
						add(TokenType::jumpID, content.substr(idIndex, idSize));
						advance(0);
						break;
					}
					addText(original);
					advance(0);
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
				parseSpace();
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
					add(TokenType::headingIDOper, ">", beginPosition);

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

	void Lexer::parseBlock() {
		Position beginPosition = position;

		size_t currentIndent = is(0, '\t') ? count('\t') : 0;

		if (currentIndent > indent) {
			while (currentIndent > indent) {
				beginPosition.column = indent;
				add(TokenType::indentIncr, ">+", beginPosition, beginPosition);
				++indent;
			}
		}

		if (currentIndent < indent) {
			while (currentIndent < indent) {
				--indent;
				// indentDecr has no symbol
				add(TokenType::indentDecr, ">-", tokens.back().begin, tokens.back().begin);
			}
		}

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
							add(TokenType::chapterOper, ">>>", beginPosition);
							break;
						case 2:
							add(TokenType::sectionOper, ">>", beginPosition);
							break;
						case 1:
							add(TokenType::subsectionOper, ">", beginPosition);
							break;
					}
					break;
				}
				addText(std::string(counter, '>'));
				break;
			}

			case '-': {
				parseCode();
				break;
			}

			case '[':
				if (check(3) && is(2, ']') && is(3, ' ')) {
					if (is(1, ' ')) {
						add(TokenType::checkbox, "[ ]");
						advance(3);
						break;
					}
					if (is(1, 'v')) {
						add(TokenType::checkbox, "[v]");
						advance(2);
						break;
					}
					if (is(1, 'x')) {
						add(TokenType::checkbox, "[x]");
						advance(2);
						break;
					}
				}
				
				// see inline [
				break;

			case '|':
				parseTable();
				break;

			case '<':
				if (check(3) && is(2, '>') && is(3, ' ')) {
					// </> Note
					// <?> Hint
					// <!> Important
					// <^> Warning
					// <@> Danger
					// <&> See also
					if (is(1, '/')) {
						advance(2);
						parseSpace();
						add(TokenType::admon, "</>");
						break;
					}
					if (is(1, '?')) {
						advance(2);
						parseSpace();
						add(TokenType::admon, "<?>");
						break;
					}
					if (is(1, '!')) {
						advance(2);
						parseSpace();
						add(TokenType::admon, "<!>");
						break;
					}
					if (is(1, '^')) {
						advance(2);
						parseSpace();
						add(TokenType::admon, "<^>");
						break;
					}
					if (is(1, '@')) {
						advance(2);
						parseSpace();
						add(TokenType::admon, "<@>");
						break;
					}
					if (is(1, '&')) {
						advance(2);
						parseSpace();
						add(TokenType::admon, "<&>");
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
				std::string number;
				while (check(0) && get(0) >= '0' && get(0) <= '9') {
					number += get(0);
					advance(0);
				}

				if (check(1) && is(0, '.') && is(1, ' ')) {
					advance(0);
					parseSpace();
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

	void Lexer::parseCode() {
		Position beginPosition = position;

		size_t openingIndent = indent;
		size_t openingCounter = count('-');

		if (openingCounter == 1) {
			if (is(0, ' ')) {
				advance(0);
				add(TokenType::bullet, "-", beginPosition);
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

			while (check(0) && !is(0, '\n') && isSymbol(0)) {
				++langSize;
				advance(0);
			}
			
			if (langSize == 0 || !is(0, '\n')) {
				retreat(openingCounter + spaceCounter + langSize);
				return;
			}

			add(TokenType::codeMarker, std::string(openingCounter, '-'));
			add(TokenType::codeLang, content.substr(langIndex, langSize));
		} else {
			if (!is(0, '\n')) {
				retreat(openingCounter + spaceCounter);
				// see inline -
				return;
			}

			add(TokenType::codeMarker, std::string(openingCounter, '-'));
		}

		std::string source;

		while (check(0)) {
			if (is(0, '\n')) {
				size_t newline = count('\n');
				size_t indent = count('\t');

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

		add(TokenType::codeSource, source.substr(trimBegin, trimSize));
		add(TokenType::codeMarker, std::string(openingCounter, '-'));
	}

	void Lexer::parseSpace() {
		while (check(0) && is(0, ' ')) {
			advance(0);
		}

		return;
	}

	void Lexer::parseTable() {
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

	void Lexer::parseText() {
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

	void Lexer::parseQuoMark(bool should, TokenType leftType, const std::string& leftValue, TokenType rightType, const std::string& rightValue) {
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
