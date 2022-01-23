#include "Lexer.hpp"
#include "IO.hpp"

namespace Gularen
{
	Lexer::Lexer()
	{
		reset();
	}

	void Lexer::setBuffer(const std::string& buffer)
	{
		this->buffer = buffer;
		bufferSize = buffer.size();

		reset();
	}

	void Lexer::setTokens(const std::vector<Token>& tokens)
	{
		reset();
		buffer.clear();

		this->tokens = tokens;
	}

	void Lexer::parse()
	{
		reset();
		add(Token(TokenType::openDocument));
		parseNewlineRules();

		while (isValid()) {
			if (parseGlobalRules()) {
			} else if (parseNewlineRules()) {
			} else if (parseFormattingRules()) {
			} else if (parseQuotingRules()) {
			} else if (parseWriter()) {
			} else {
				if (isValidBuffer()) {
					consumeBuffer();
				} else {
					add(Token(TokenType::buffer, std::string(1, getCurrentByte())));
					skip();
				}
			}
		}

		add(Token(TokenType::closeDocument));
	}

	std::string Lexer::getBuffer()
	{
		return buffer;
	}

	Token& Lexer::getTokenAt(size_t index)
	{
		return tokens[index];
	}

	void Lexer::reset()
	{
		inHeaderLine = false;
		inInterpolatedBuffer = false;
		inCodeBlock = false;

		bufferIndex = 0;
		tokens.clear();

		previousByte = 0;
		previousType = TokenType::unknown;

		currentDepth = 0;
	}

	std::vector<Token>& Lexer::getTokens()
	{
		return tokens;
	}

	std::string Lexer::getTokensAsString()
	{
		std::string buffer;

		for (Token& token: tokens)
			buffer += token.ToString() + "\n";

		return buffer + "\n";
	}

	bool Lexer::parseGlobalRules()
	{
		switch (getCurrentByte()) {
			case '\r':
				skip();
				return true;

			case '~':
				while (isValid() && getCurrentByte() != '\n')
					skip();
				return true;

			case '>':
				if (inHeaderLine) {
					return parseSymbolOrString();
				}
				skip();
				return true;

			case '<':
				add(Token(TokenType::reverseTail, 1));
				skip();
				return true;

			case '\\':
				skip();
				add(Token(TokenType::buffer, std::string(1, getCurrentByte())));
				skip();
				return true;

			case '|':
				skip();
				if (!tokens.empty() && tokens.back().type == TokenType::buffer) {
					// remove blank spaces of previous token
					for (size_t i = tokens.back().value.size() - 1; i >= 0; --i) {
						if (tokens.back().value[i] != ' ') {
							tokens.back().value = tokens.back().value.substr(0, i + 1);
							break;
						}
					}
				}
				add(Token(TokenType::pipe));
				skipSpaces();
				return true;

			case '#':
				consumeTag(TokenType::hash);
				break;

			case '@':
				consumeTag(TokenType::at);
				break;

			default:
				return false;
		}
	}

	bool Lexer::parseFormattingRules()
	{
		switch (getCurrentByte()) {
			case '*':
				add(Token(TokenType::asterisk));
				skip();
				return true;

			case '_':
				add(Token(TokenType::underline));
				skip();
				return true;

			case '`':
				add(Token(TokenType::backtick));
				skip();
				return true;

			default:
				return false;
		}
	}

	void Lexer::consumeNewline()
	{
		size_t size = 0;
		while (isValid() && getCurrentByte() == '\n') {
			++size;
			skip();
		}

		if (!tokens.empty() && tokens.back().type == TokenType::newline)
			tokens.back().size += size;
		else {
			Token token(TokenType::newline);
			token.size = size;
			add(static_cast<Token&&>(token));
		}
	}

	void Lexer::consumeBuffer()
	{
		std::string buffer;

		while (isValid() && isValidBuffer()) {
			buffer += getCurrentByte();
			skip();
		}

		if (!tokens.empty() && tokens.back().type == TokenType::buffer)
			tokens.back().value += buffer;
		else
			add(Token(TokenType::buffer, buffer));
	}

	bool Lexer::parseQuotingRules()
	{
		switch (getCurrentByte()) {
			case '\'':
				consumeQuote(TokenType::openDoubleQuote,
					TokenType::openSingleQuote,
					TokenType::closeSingleQuote);
				return true;

			case '"':
				consumeQuote(TokenType::openSingleQuote,
					TokenType::openDoubleQuote,
					TokenType::closeDoubleQuote);
				return true;

			default:
				return false;
		}
	}

	void Lexer::consumeQuote(TokenType previousType, TokenType openType, TokenType closeType)
	{
		char previousByte = getPreviousByte();

		add(Token((
			previousByte == ' ' ||
			previousByte == '\t' ||
			previousByte == '\n' ||
			previousByte == '\0' ||
			this->previousType == previousType)
				? openType
				: closeType));
		skip();
	}

	bool Lexer::parseNewlineRules()
	{
		if (getCurrentByte() != '\n')
			return false;

		consumeNewline();

		// state variables
		inHeaderLine = false;

		if (getCurrentByte() == ' ') {
			Token token(TokenType::space);
			token.size = 1;
			skip();

			while (isValid() && getCurrentByte() == ' ') {
				++token.size;
				skip();
			}

			currentDepth = token.size;
			add(static_cast<Token&&>(token));
		} else
			currentDepth = 0;

		switch (getCurrentByte()) {
			case '-': {
				size_t size = 0;

				while (isValid() && getCurrentByte() == '-') {
					skip();
					++size;
				}

				if (size == 1) {
					if (getCurrentByte() == '>' && getNextByte() == ' ') {
						add(Token(TokenType::arrow, 1));
						skip();
						skipSpaces();
					} else {
						add(Token(TokenType::bullet));
						skipSpaces();
					}
				} else if (size == 2 && getCurrentByte() == '>' && getNextByte() == ' ') {
					add(Token(TokenType::arrow, 2));
					skip();
					skipSpaces();
				} else {
					add(Token(TokenType::line, size));
					skipSpaces();

					if (inCodeBlock) {
						std::string buffer;

						while (isValid()) {
							if (getCurrentByte() == '\n') {
								skip();
								// skip identations or blank lines
								size_t shouldSkipCounter = 0;
								while (isValid() && shouldSkipCounter < currentDepth && getCurrentByte() != '\n') {
									skip();
									++shouldSkipCounter;
								}

								if (getCurrentByte() == '-') {
									size_t laterSize = 0;

									while (isValid() && getCurrentByte() == '-') {
										++laterSize;
										skip();
									}

									if (laterSize == size) {
										add(Token(TokenType::buffer, buffer));
										add(Token(TokenType::line, size));
										inCodeBlock = false;
										break;
									} else {
										if (!buffer.empty())
											buffer += "\n";
										buffer += std::string(laterSize, '-');
									}
								} else if (!buffer.empty())
									buffer += "\n";
							} else {
								buffer += getCurrentByte();
								skip();
							}
						}
					}
				}
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
			case '9':
			case '0': {
				std::string buffer;

				while (isValid() && isValidNumeric()) {
					buffer += getCurrentByte();
					skip();
				}

				if (getCurrentByte() == '.' && getNextByte() == ' ') {
					add(Token(TokenType::numericBullet));
					skip(2);
				} else {
					if (!tokens.empty() && tokens.back().type == TokenType::buffer)
						tokens.back().value += buffer;
					else
						add(Token(TokenType::buffer, buffer));
				}

				break;
			}

			case '.':
				if (getNextByte(1) == '.' && getNextByte(2) == ' ') {
					add(Token(TokenType::numericBullet));
					skip(3);
				}
				break;

			case '[':
				if (getNextByte(2) == ']') {
					Token token(TokenType::checkBox);

					switch (getNextByte(1)) {
						case ' ':
							token.size = 1;
							add(static_cast<Token&&>(token));
							skip(3);
							skipSpaces();
							break;

						case '-':
							token.size = 2;
							add(static_cast<Token&&>(token));
							skip(3);
							skipSpaces();
							break;

						case '+':
							token.size = 3;
							add(static_cast<Token&&>(token));
							skip(3);
							skipSpaces();
							break;

						default:
							break;
					}
				}
				break;

			case '>': {
				inHeaderLine = true;

				skip();
				if (getCurrentByte() == ' ') {
					add(Token(TokenType::tail, 1));
					skipSpaces();
					break;
				} else if (getCurrentByte() == '>') {
					skip();
					if (getCurrentByte() == ' ') {
						add(Token(TokenType::tail, 2));
						skipSpaces();
					} else if (getCurrentByte() == '>' && getNextByte() == ' ') {
						add(Token(TokenType::tail, 3));
						skip();
						skipSpaces();
					} else if (getCurrentByte() == '-' && getNextByte(1) == '>' && getNextByte(2) == ' ') {
						add(Token(TokenType::arrow, 3));
						skip(2);
						skipSpaces();
					} else if (getCurrentByte() == '-' && getNextByte(1) == '-' && getNextByte(2) == '>' &&
						getNextByte(3) == ' ') {
						add(Token(TokenType::arrow, 4));
						skip(3);
						skipSpaces();
					}
				} else if (getCurrentByte() == '-') {
					skip();
					if (getCurrentByte() == '>' && getNextByte() == ' ') {
						add(Token(TokenType::arrow, 2));
						skip();
						skipSpaces();
					}
				}
			}

			case '<':
				if (getNextByte() == '<') {
					skip(2);
					if (getCurrentByte() == '<') {
						add(Token(TokenType::reverseTail, 3));
						skip();
					} else
						add(Token(TokenType::reverseTail, 2));
				}
				break;

			case '$':
				add(Token(TokenType::dollar));
				skip();
				skipSpaces();
				consumeBlockKeyword();
				break;
		}

		return true;
	}

	void Lexer::consumeSymbol()
	{
		while (isValid() && (isValidSymbol() || getCurrentByte() == '.')) {
			if (getCurrentByte() == '.') {
				add(Token(TokenType::period));
				skip();
			} else {
				std::string symbol;
				while (isValid() && isValidSymbol()) {
					symbol += getCurrentByte();
					skip();
				}
				add(Token(TokenType::symbol, symbol));
			}
		}
	}

	void Lexer::consumeString()
	{
		Token token(TokenType::buffer);
		skip();

		while (isValid() && isValidString()) {
			if (getCurrentByte() == '\\')
				skip();

			token.value += getCurrentByte();
			skip();
		}

		add(static_cast<Token&&>(token));
		skip();
	}

	void Lexer::consumeFormattedString()
	{
		add(Token(TokenType::openStyleQuote));
		skip();

		while (isValid() && getCurrentByte() != '"') {
			switch (getCurrentByte()) {
				case '*':
					add(Token(TokenType::asterisk));
					skip();
					break;

				case '_':
					add(Token(TokenType::underline));
					skip();
					break;

				case '`':
					add(Token(TokenType::backtick));
					skip();
					break;

				case '\'':
					consumeQuote(TokenType::openDoubleQuote,
						TokenType::openSingleQuote,
						TokenType::closeSingleQuote);
					break;

				case '"':
					consumeQuote(TokenType::openSingleQuote,
						TokenType::openDoubleQuote,
						TokenType::closeDoubleQuote);
					break;

				default:
					Token token(TokenType::buffer);
					while (isValid() && isValidFormattedString()) {
						if (getCurrentByte() == '\\')
							skip();

						token.value += getCurrentByte();
						skip();
					}
					add(static_cast<Token&&>(token));
					break;
			}
		}

		add(Token(TokenType::closeStyleQuote));
		skip();
	}

	void Lexer::consumeArray()
	{
	}

	void Lexer::consumeArguments()
	{
	}

	bool Lexer::parseFunction()
	{
		switch (getCurrentByte()) {
			case ':':
				add(Token(TokenType::colon));
				skip();
				return true;

			case '!':
				add(Token(TokenType::exclamationMark));
				skip();
				return true;

			case '?':
				add(Token(TokenType::questionMark));
				skip();
				return true;

			case '^':
				add(Token(TokenType::caret));
				skip();
				return true;

			case '&':
				add(Token(TokenType::ampersand));
				skip();
				return true;

			default:
				return false;
		}
	}

	void Lexer::consumeBlockKeyword()
	{
		std::string symbol;

		while (isValid() && isValidSymbol()) {
			symbol += getCurrentByte();
			skip();
		}

		if (symbol == "image") {
			add(Token(TokenType::imageKeyword));
			skipSpaces();
		} else if (symbol == "table") {
			add(Token(TokenType::tableKeyword));
			skipSpaces();
		} else if (symbol == "code") {
			add(Token(TokenType::codeKeyword));
			skipSpaces();
			inCodeBlock = true;
		} else if (symbol == "admonition") {
			add(Token(TokenType::admonitionKeyword));
			skipSpaces();
		} else if (symbol == "file") {
			add(Token(TokenType::fileKeyword));
			skipSpaces();
		} else if (symbol == "toc") {
			add(Token(TokenType::tocKeyword));
			skipSpaces();
		} else if (symbol == "index") {
			add(Token(TokenType::tocKeyword));
			skipSpaces();
		} else if (symbol == "reference") {
			add(Token(TokenType::referenceKeyword));
			skipSpaces();
		} else
			add(Token(TokenType::symbol, symbol));

		skipSpaces();

		if (getCurrentByte() == '\'')
			consumeString();
		else if (getCurrentByte() == '"')
			consumeFormattedString();
		else if (isValidSymbol()) {
			std::string symbol;
			while (isValid() && isValidSymbol()) {
				symbol += getCurrentByte();
				skip();
			}
			add(Token(TokenType::symbol, symbol));
		}

		skipSpaces();

		if (getCurrentByte() == '{') {
			add(Token(TokenType::openCurlyBracket));
			skip();
		}
	}

	bool Lexer::isValid()
	{
		return bufferIndex < bufferSize;
	}

	bool Lexer::isValidBuffer()
	{
		char c = getCurrentByte();

		return (c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			(c >= '0' && c <= '9') ||
			c == ' ' || c == '-' || c == '.' ||
			c == '!' || c == '?' ||
			c == ';' || c == ':';
	}

	bool Lexer::isValidString()
	{
		return getCurrentByte() != '\'';
	}

	bool Lexer::isValidFormattedString()
	{
		char c = getCurrentByte();

		return (c != '"' && c != '*' && c != '_' && c != '`');
	}

	bool Lexer::isValidSymbol()
	{
		char c = getCurrentByte();

		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
	}

	bool Lexer::isValidNumeric()
	{
		char c = getCurrentByte();

		return c >= '0' && c <= '9';
	}

	char Lexer::getCurrentByte()
	{
		return buffer[bufferIndex];
	}

	char Lexer::getPreviousByte()
	{
		return previousByte;
	}

	char Lexer::getNextByte(size_t offset)
	{
		return bufferIndex + offset < bufferSize ? buffer[bufferIndex + offset] : 0;
	}

	void Lexer::skip(size_t offset)
	{
		previousByte = getCurrentByte();
		bufferIndex += offset;
	}

	void Lexer::skipSpaces()
	{
		while (isValid() && getCurrentByte() == ' ')
			skip();
	}

	void Lexer::add(Token&& token)
	{
		previousType = token.type;
		tokens.emplace_back(token);
	}
	
	bool Lexer::parseWriter()
	{
		if (getCurrentByte() != '{')
			return false;

		skip();

		if (parseSymbolOrStringOrFormattedString() || parseFunction()) {
			if (getCurrentByte() != '}')
				return false;

			add(Token(TokenType::closeCurlyBracket));
			skip();
			return true;
		}

		return false;
	}


	bool Lexer::parseSymbolOrString()
	{
		if (getCurrentByte() == '\'') {
			consumeString();
			return true;
		}

		if (isValidSymbol()) {
			consumeSymbol();
			return true;
		}

		return false;
	}

	bool Lexer::parseSymbolOrStringOrFormattedString()
	{
		if (parseSymbolOrString())
			return true;

		if (isValidFormattedString()) {
			consumeFormattedString();
			return true;
		}

		return false;
	}
	void Lexer::consumeTag(TokenType familyType)
	{
		skip();
		Token token(familyType);
		while (isValid() && isValidSymbol()) {
			token.value += getCurrentByte();
			skip();
		}
		add(static_cast<Token&&>(token));
	}
}