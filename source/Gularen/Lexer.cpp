#include "Lexer.h"
#include <iostream>

namespace Gularen {
	const Tokens& Lexer::parse(const std::string& content) {
		this->tokens.clear();
		this->content = content;
		this->index = 0;

		parseBlock();

		while (check(0)) {
			parseInline();
		}

		return tokens;
	}

	bool Lexer::check(size_t offset) {
		return index + offset < content.size();
	}

	bool Lexer::is(size_t offset, char c) {
		return content[index + offset] == c;
	}

	char Lexer::get(size_t offset) {
		return content[index + offset];
	}

	void Lexer::advance(size_t offset) {
		index += 1 + offset;
	}

	size_t Lexer::count(char c) {
		size_t count = 0;
		while (check(0) && is(0, c)) {
			++count;
			advance(0);
		}
		return count;
	}

	void Lexer::add(TokenType type, size_t count, const std::string& value) {
		Token token;
		token.type = type;
		token.value = value;
		token.count = count;
		tokens.push_back(token);
	}

	void Lexer::addText(const std::string value) {
		if (!tokens.empty() && tokens.back().type == TokenType::text) {
			tokens.back().value += value;
		} else {
			add(TokenType::text, 1, value);
		}
	}

	void Lexer::parseInline() {
		switch (get(0)) {
			case '\n':
				add(TokenType::newline, count('\n'), "\\n");
				parseBlock();
				break;

			case '\\':
				advance(0);
				if (check(0)) {
					addText(content.substr(index));
					advance(0);
				}
				break;

			case '~':
				advance(0);

				while (check(0) && !is(0, '\n')) {
					advance(0);
				}
				break;

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

			case '<':
				if (check(1) && is(1, '<')) {
					add(TokenType::break_, 2, "<<");
					advance(1);
					break;
				}
				add(TokenType::break_, 1, "<");
				advance(0);
				break;

			case '*':
				if (check(2) && is(1, '*') && is(2, '*')) {
					add(TokenType::dinkus, 1, "***");
					advance(2);
					break;
				}
				add(TokenType::fsBold, 1, "*");
				advance(0);
				break;

			case '_':
				add(TokenType::fsItalic, 1, "_");
				advance(0);
				break;

			case '`':
				add(TokenType::fsMonospace, 1, "`");
				advance(0);
				break;

			case '>':
				if (check(1) && is(1, ' ')) {
					advance(0);
					parseSpace();
					add(TokenType::headingIDMarker, 1, ">");

					size_t idIndex = index;
					size_t idSize = 0;

					while (check(0) && !is(0, '\n')) {
						++idSize;
						advance(0);
					}
					
					if (idSize > 0) {
						add(TokenType::headingID, 1, content.substr(idIndex, idSize));
					}
				}

				addText(">");
				advance(0);
				break;

			default:
				advance(0);
				break;
		}
	}

	void Lexer::parseBlock() {
		switch (get(0)) {
			case '\t':
				add(TokenType::indent, count('\t'), "\\t");
				parseBlock();
				break;

			case '>': {
				size_t counter = count('>');
				if (counter > 3) {
					addText(std::string(counter, '>'));
					break;
				}

				if (is(0, ' ')) {
					parseSpace();
					add(TokenType::headingMarker, counter, std::string(counter, '>'));
					break;
				}
				break;
			}

			case '-': {
				parseCode();
				break;
			}
			
			default:
				break;
		}
	}

	void Lexer::parseCode() {
		size_t counter = count('-');

		if (counter == 1) {
			if (is(0, ' ')) {
				parseSpace();
				add(TokenType::bullet, 1, "-");
				return;
			}
			addText("-");
			return;
		}
		
		if (counter == 2) {
			addText("-");
			return;
		}

		add(TokenType::codeMarker, 1, std::string(counter, '-'));
		parseSpace();

		size_t langIndex = index;
		size_t langSize = 0;

		while (check(0) && !is(0, '\n')) {
			++langSize;
			advance(0);
		}
		
		if (langSize > 0) {
			add(TokenType::codeLang, 1, content.substr(langIndex, langSize));
		}
		
		if (!is(0, '\n')) {
			return;
		}

		advance(0);
		std::string source;

		while (check(0)) {
			if (is(0, '\n')) {
				size_t newlineCounter = count('\n');

				if (check(0)) {
					size_t tabCounter = count('\t');

					if (check(2) && is(0, '-') && is(1, '-') && is(2, '-')) {
						size_t closingCounter = count('-');
						if (closingCounter == counter) {
							break;
						}
						source += std::string(closingCounter, '-');
					} else {
						if (newlineCounter > 0) {
							source += std::string(newlineCounter, '\n');
						}

						if (tabCounter > 0) {
							source += std::string(tabCounter, '\t');
						}
					}
				} else {
					break;
				}
			}

			source += get(0);
			advance(0);
		}

		add(TokenType::codeSource, 1, source);
		add(TokenType::codeMarker, 1, std::string(counter, '-'));
	}

	void Lexer::parseSpace() {
		while (check(0) && is(0, ' ')) {
			advance(0);
		}

		return;
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
}
