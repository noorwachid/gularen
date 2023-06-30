#include <Gularen/Lexer.h>
#include <iostream>
#include <fstream>

namespace Gularen {
	void Lexer::load(const std::string& path) {
		std::ifstream file(path);
		file.seekg(0, std::ios::end);   
		content.reserve(file.tellg());
		file.seekg(0, std::ios::beg);
		content.assign(
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
		);
	}

	void Lexer::set(const std::string& content) {
		this->content = content;
	}

	void Lexer::parse() {
		if (content.empty()) return;

		this->tokens.clear();
		this->index = 0;

		parseBlock();

		while (check(0)) {
			parseInline();
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

			case '\n':
				// from comment
				if (!tokens.empty() && tokens.back().type == TokenType::newline) {
					tokens.back().count += count('\n');
				} else {
					add(TokenType::newline, count('\n'), "\\n");
				}
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
			
			case '\'':
				parseQuoMark(tokens.empty() || tokens.back().type == TokenType::ldQuo, TokenType::lsQuo, "‘", TokenType::rsQuo, "’");
				advance(0);
				break;

			case '"':
				parseQuoMark(tokens.empty() || tokens.back().type == TokenType::lsQuo, TokenType::ldQuo, "“", TokenType::rdQuo, "”");
				advance(0);
				break;

			case '-':
				if (check(1) && get(1) >= '0' && get(1) <= '9') {
					add(TokenType::minus, 1, "−");
					advance(0);
					break;
				}
				if (check(2) && is(1, '-') && is(2, '-')) {
					advance(2);
					add(TokenType::emDash, 1, "—");
					break;
				}
				if (check(1) && is(1, '-')) {
					advance(1);
					add(TokenType::enDash, 1, "–");
					break;
				}
				advance(0);
				add(TokenType::hyphen, 1, "-");
				break;

			case ':': {
				advance(0);

				if (check(0)) {
					size_t emojiIndex = index;
					size_t emojiSize = 0;

					while (check(0) && (get(0) >= 'a' && get(0) <= 'z' || is(0, '-'))) {
						if (is(0, ':')) {
							break;
						}

						advance(0);
						++emojiSize;
					}

					if (is(0, ':') && emojiSize > 0) {
						advance(0);
						add(TokenType::emoji, 1, content.substr(emojiIndex, emojiSize));
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
						add(TokenType::resource, 1, resource.substr(0, idMarkerIndex));
						add(TokenType::resourceID, 1, resource.substr(idMarkerIndex + 1));
					} else {
						add(TokenType::resource, 1, resource);
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

							add(TokenType::resourceLabel, 1, label);
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
					add(TokenType::presentMarker, 1, "!");
					// see inline [
					break;
				}
				advance(0);
				addText("!");
				break;

			case '?':
				if (check(1) && is(1, '[')) {
					advance(0);
					add(TokenType::includeMarker, 1, "?");
					// see inline [
					break;
				}
				advance(0);
				addText("!");
				break;

			case '^':
				if (check(1) && is(1, '[')) {
					advance(0);
					add(TokenType::jumpMarker, 1, "^");
					// see inline [
					break;
				}
				advance(0);
				addText("!");
				break;

			case '=':
				if (check(1) && is(1, '[')) {
					advance(0);
					add(TokenType::describeMarker, 1, "=");
					// see inline [
					break;
				}
				advance(0);
				addText("!");
				break;

			case '|':
				add(TokenType::pipe, 1, "|");
				advance(0);
				parseSpace();
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
					break;
				}

				addText(">");
				advance(0);
				break;

			default:
				addText(content.substr(index, 1));
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

			case '[':
				if (check(2) && is(2, ']')) {
					if (is(1, ' ')) {
						add(TokenType::checkbox, 0, "[ ]");
						break;
					}
					if (is(1, 'v')) {
						add(TokenType::checkbox, 1, "[v]");
						break;
					}
					if (is(1, 'x')) {
						add(TokenType::checkbox, 2, "[x]");
						break;
					}
				}
				
				// see inline [
				break;

			case '.': {
				if (check(2) && is(1, '.') && is(2, ' ')) {
					advance(1);
					parseSpace();
					add(TokenType::index, 1, "..");
					break;
				}
				
				addText(".");
				break;
			}

			case '|':
				parseTable();
				break;

			case '/': {
				size_t depth = 0;
				while (check(0) && (is(0, '/') || is(0, ' '))) {
					if (is(0, '/')) {
						++depth;
					}
					advance(0);
				}
				add(TokenType::blockquote, depth, "/");
				break;
			}

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
						add(TokenType::admon, 1, "</>");
						break;
					}
					if (is(1, '?')) {
						advance(2);
						parseSpace();
						add(TokenType::admon, 2, "<?>");
						break;
					}
					if (is(1, '!')) {
						advance(2);
						parseSpace();
						add(TokenType::admon, 3, "<!>");
						break;
					}
					if (is(1, '^')) {
						advance(2);
						parseSpace();
						add(TokenType::admon, 4, "<^>");
						break;
					}
					if (is(1, '@')) {
						advance(2);
						parseSpace();
						add(TokenType::admon, 5, "<@>");
						break;
					}
					if (is(1, '&')) {
						advance(2);
						parseSpace();
						add(TokenType::admon, 6, "<&>");
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
					add(TokenType::index, std::stoi(number), number + ".");
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
		size_t openingIndent = !tokens.empty() && tokens.back().type == TokenType::indent ? tokens.back().count : 0;
		size_t openingCounter = count('-');

		if (openingCounter == 1) {
			if (is(0, ' ')) {
				parseSpace();
				add(TokenType::bullet, 1, "-");
				return;
			}

			--index;
			// see inline -
			return;
		}
		
		if (openingCounter == 2) {
			--index;
			--index;
			// see inline -
			return;
		}

		add(TokenType::codeMarker, 1, std::string(openingCounter, '-'));
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

		std::string source;

		while (check(0)) {
			if (is(0, '\n')) {
				size_t newline = count('\n');
				size_t indent = count('\t');

				if (check(2) && is(0, '-') && is(1, '-') && is(2, '-')) {
					size_t closingCounter = count('-');
					if (check(0) && is(0, '\n') && closingCounter == openingCounter) {
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

		add(TokenType::codeSource, 1, source.substr(trimBegin, trimSize));
		add(TokenType::codeMarker, 1, std::string(openingCounter, '-'));
	}

	void Lexer::parseSpace() {
		while (check(0) && is(0, ' ')) {
			advance(0);
		}

		return;
	}

	void Lexer::parseTable() {
		add(TokenType::pipe, 1, "|");
		advance(0);
		parseSpace();

		if (!is(0, ':') && !is(0, '-')) {
			return;
		}

		while (check(0) && !is(0, '\n')) {
			if (check(1) && is(0, ':') && is(1, '-')) {
				advance(0);
				size_t lineCounter = count('-');

				if (get(0) == ':') {
					advance(0);
					add(TokenType::pipeConnector, 1, ":-:");
					continue;
				}

				add(TokenType::pipeConnector, 0, ":--");
				continue;
			}

			if (is(0, '-')) {
				size_t lineCounter = count('-');

				if (get(0) == ':') {
					advance(0);
					add(TokenType::pipeConnector, 2, "--:");
					continue;
				}

				add(TokenType::pipeConnector, 0, ":--");
				continue;
			}

			if (is(0, '|')) {
				add(TokenType::pipe, 1, "|");
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
            add(leftType, 1, leftValue);
            return;
        }

        add(rightType, 1, rightValue);
	}
}
